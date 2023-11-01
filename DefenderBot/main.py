# Franck Limtung
# 2023-Oct
# A reddit bot to argue with people using ChatGPT
# TODO:
# Free usage of chatgpt with usage tracking per user
# add customization through sending messages
# More robust error catching and recovery
import os
import time
import re
import logging
import praw
import prawcore
import tiktoken
from sqlalchemy.orm import Session
from sqlalchemy import select, text
import botDatabase as botDB
import botLogger
import constants
import botAI

def get_context_url(comment):
    return comment.context.split('?', 1)[0] # Remove weird context thing at the end

def read_inbox(reddit, conn, logger):
    logger.info("Reading inbox")

    # Grab mentions, messages, and replies
    for unread in reddit.inbox.unread():
        if unread.author == None or unread.author.name in constants.BLACKLIST:
            unread.mark_read()
            continue

        # Add username if it does not exist
        result = botDB.add_user(unread.author, conn)
        if result.rowcount > 0:
            logger.info("Added new user " + unread.author.name)

        # Messages are handled manually by bot manager
        if isinstance(unread, praw.models.Message):
            continue

        # Comment replies are added to comments
        elif unread.parent().author.name == constants.BOT_USERNAME:
            # Set to need_to_respond if the parent is a watched post
            logger.info("Adding comment reply")
            insertResult = conn.execute(text("""INSERT  INTO COMMENTS (REDDIT_ID, TEXT, USER_ID, IS_SUBMISSION)
                                                VALUES  (:id, :txt, :uid, 0);"""),
                                        {'id': unread.id, 'txt': unread.body, 'uid': botDB.get_id(unread.author.name, conn)})
            if botDB.comment_in_watchlist(unread.parent().id, conn):
                parent_id = conn.execute(text("""SELECT    ID
                                                 FROM      COMMENTS
                                                 WHERE     REDDIT_ID = :pid;"""), {'pid': unread.parent().id}).fetchone()._mapping["id"]
                conn.execute(text("""UPDATE    COMMENTS
                                     SET       NEED_TO_EVALUATE = 1, PARENT_ID = :pid
                                     WHERE     ID = :id;"""), {'pid': parent_id, 'id': insertResult.lastrowid});

        # Mentions are used to add to watchlist
        else:
            if unread.subreddit.name in constants.SUB_BLACKLIST:
                logger.info("Mention was rejected for being from a blacklisted sub.")
                unread.mark_read()
                continue;

            user_id = botDB.get_id(unread.author.name, conn)
            result = conn.execute(text("""INSERT
                                          OR
                                          IGNORE    INTO COMMENTS (REDDIT_ID, TEXT, USER_ID, IS_SUBMISSION)
                                          VALUES    (:id, :txt, :uid, 0);"""), {'id': unread.id, 'txt': unread.body, 'uid': user_id})

            # Add parent to watchlist if the user is creator of parent
            parent = unread.parent()
            if result.rowcount > 0:
                deny_reason = "NO_ERROR"
                if not isinstance(parent, praw.models.Submission):
                    deny_reason = "User asked to guard a comment which is not supported."
                elif parent.author.name != unread.author.name:
                    deny_reason = "User is not the author of the parent submission."
                elif parent.author.name in constants.BLACKLIST:
                    deny_reason = "User asked to guard submission from disallowed user"
                elif botDB.comment_in_watchlist(parent.id, conn):
                    deny_reason = "User asked to guard submission which is already guarded."
                else:
                    logger.info("Adding new watched submission")

                    prompt = ""
                    # Extract prompt by removing other stuff
                    if ("prompt" in unread.body.lower()):
                        prompt = re.sub(r'(^.*?prompt(:|)\s*)', repl = '', string = unread.body, flags = re.DOTALL | re.I)
                    result = conn.execute(text("INSERT INTO WATCHLIST (PROMPT, USER_ID) VALUES (:p, :uid);"), {'p': prompt, 'uid': user_id})

                    watch_id = result.lastrowid
                    # Add parent to comments
                    # Note: add title as well using Markdown, which ChatGPT understands
                    conn.execute(text("""INSERT
                                         OR
                                         REPLACE    INTO COMMENTS (REDDIT_ID, TEXT, USER_ID, WATCH_ID, IS_SUBMISSION)
                                         VALUES     (:rid, :ptxt, :uid, :wid, 1);"""),
                                 {'rid': parent.id, 'ptxt': "#" + parent.title + "\n" + parent.selftext, 'uid': user_id, 'wid': watch_id})

                    try:
                        bot_reply = unread.reply(constants.BOT_TRIGGER_REPLY)
                    except Exception as e:
                        logger.info("Creating reply failed. Error: " + str(e))

                    conn.execute(text("""INSERT     INTO COMMENTS (REDDIT_ID, TEXT, USER_ID, WATCH_ID, IS_SUBMISSION, PARENT_ID)
                                         VALUES     (:rid, :txt, :uid, :wid, 0, :pid);"""), {
                                 'rid': bot_reply.id, 'txt': bot_reply.body, 'uid': botDB.get_id(constants.BOT_USERNAME, conn),
                                 'wid': watch_id, 'pid': unread.id})

                # Inform user that it didn't work if needed
                if deny_reason != "NO_ERROR":
                    try:
                        reddit.redditor(unread.author.name).message(subject="Bot Activation Failed", message="Activation requested here: " + get_context_url(unread) + " failed. Reason: " + deny_reason)
                    except Exception as e:
                        logger.info("Creating message failed. Error: " + str(e))

                    logger.info(unread.author.name + " was denied from adding a comment to the watchlist. Reason: " + deny_reason)

        unread.mark_read()

    conn.commit()
    logger.info("Finished reading inbox")

def read_submission_replies(reddit, conn, logger):
    logger.info("Looking for new replies to submissions")

    result = conn.execute(text("""SELECT    REDDIT_ID
                                  FROM      COMMENTS
                                  WHERE     WATCH_ID IS NOT NULL AND IS_SUBMISSION = 1;""")).all()
    for thread_id in result:
        count = 0; # Count of new comments found
        submission = reddit.submission(id = thread_id[0])
        submission.comment_sort = "new"
        try:
            submission.comments.replace_more(limit=0)
            logger.debug(f"Fetched {len(submission.comments)} comments in submission {submission.id}")
            for comment in submission.comments:
                if comment.author == None or comment.author.name in constants.BLACKLIST:
                    continue

                result = botDB.add_user(comment.author, conn)
                if result.rowcount > 0:
                   logger.info("Added new user " + comment.author.name)

                user_id = botDB.get_id(comment.author.name, conn)
                parent_id = conn.execute(text("""SELECT    ID
                                                 FROM      COMMENTS
                                                 WHERE     REDDIT_ID = :rid;"""), {'rid': thread_id[0]}).fetchone()._mapping["id"]
                result = conn.execute(text("""INSERT
                                              OR
                                              IGNORE    INTO COMMENTS (REDDIT_ID, TEXT, USER_ID, IS_SUBMISSION, NEED_TO_EVALUATE, PARENT_ID)
                                              VALUES    (:cid, :cb, :uid, 0, 1, :pid);"""),
                                      {'cid': comment.id, 'cb': comment.body, 'uid': user_id, 'pid': parent_id})
                if result.rowcount > 0:
                    count += 1

        except prawcore.exceptions.NotFound:
            logger.info("Not found error for submission id: " + submission.id)

        if count > 0:
            logger.info(f"Stored {count} new comments from submission {submission.id}")

    conn.commit()
    logger.info("Finished reading submission replies")

def cleanup_watchlists(reddit, conn, logger):
    logger.info("Cleaning up expired watchlists")
    result = conn.execute(text("""DELETE
                                  FROM      WATCHLIST
                                  WHERE     (JULIANDAY() - JULIANDAY(TIME_ADDED)) > :t
                                  RETURNING *;"""), {'t': constants.MAX_WATCHLIST_AGE})
    count = 0 # Amount deleted
    if (result != None):
        count = len(result.fetchall())
    conn.commit()
    logger.info(f"Finished cleaning up expired watchlists ({count} deleted)")

def evaluate_comments(reddit, conn, logger):
    count = 0
    reply_count = 0
    logger.info("Evaluating replies to watched comments")
    result = conn.execute(text("SELECT * FROM COMMENTS WHERE NEED_TO_EVALUATE = 1;")).fetchall()
    for row in result:
        count += 1
        comment = row._mapping
        # Check if the comment's parent is still in a watchlist. It's possible it's not if the watchlist was deactivated
        parent = conn.execute(text("SELECT * FROM COMMENTS WHERE ID = :id;"), {'id': comment.parent_id}).fetchone()._mapping
        if botDB.comment_in_watchlist(parent.reddit_id, conn) == False:
            conn.execute(text("""UPDATE COMMENTS
                                 SET    NEED_TO_EVALUATE = 0
                                 WHERE  ID = :id"""), {'id': comment.id})
            logger.debug(f"Comment {comment.reddit_id} was ignored because the parent watchlist was null")
            continue;

        watchlist = conn.execute(text("SELECT * FROM WATCHLIST WHERE ID = :id;"), {'id': parent.watch_id}).fetchone()._mapping
        # Handle detriggering, which only works for the original creator of the watchlist
        if (constants.BOT_DETRIGGER in comment.text.lower()) and (comment.user_id == watchlist.user_id):
            conn.execute(text("""DELETE
                                 FROM   WATCHLIST
                                 WHERE  ID = :id"""), {'id': watchlist.id})
            conn.execute(text("""UPDATE COMMENTS
                                 SET    NEED_TO_EVALUATE = 0
                                 WHERE  ID = :id"""), {'id': comment.id})
            try:
                reddit.comment(id = comment.reddit_id).reply(constants.BOT_DETRIGGER_REPLY)
            except Exception as e:
                logger.info("Creating reply failed. Error: " + str(e))

            logger.info(f"User {comment.reddit_id} deactivated a watchlist")
            continue;

        # Get entire thread history up to this point, as far back as possible
        enc = tiktoken.encoding_for_model(constants.AI_MODEL)
        history = ""
        original = parent
        response = comment
        system_prompt = f"You are a debate machine. You will argue with anyone.\n\nRules:\nRead the exchange, paying attention to meaning and intent. Answer this question: Do you think it's possible there is a disagreement here, based on ONLY the latest RESPONSE? Answer YES OR NO.\n\nThen, if YES, you have to continue the exchange by writing a REPLY to the latest post, refuting RESPONSE's argument.\n\nYour reply should embody the following characteristics: [[[{watchlist.prompt}]]]\n\nDo not confuse ORIGINAL and RESPONSE.\n\nYour reply should be in the first person (as if you are ORIGINAL).\n\nYour job is to debate. Do not concede easily. Do not say yes if there is agreement.\n\nExample 1:\nYES\n[[[REPLY FROM ORIGINAL]]]\n(reply here)\nExample 2:\nNO\n(nothing here)"

        index = 1
        keep_looping = True
        while keep_looping == True:
            index -= 1
            new_history = f"[[[ORIGINAL {index}]]]\n" + original.text + f"\n[[[END ORIGINAL {index}]]]\n[[[RESPONSE {index}]]]\n" + response.text + f"\n[[[END RESPONSE {index}]]]\n"
            encoded_history = enc.encode(new_history + history + system_prompt)
            if (len(encoded_history) + constants.AI_MAX_TOKENS) < 4096:
                history = new_history + history
                # Try to get parent and response higher up
                if (original.parent_id != None):
                    response = conn.execute(text("SELECT * FROM COMMENTS WHERE ID = :pid;"), {'pid': original.parent_id}).fetchone()._mapping
                    if (response.parent_id != None):
                        original = conn.execute(text("SELECT * FROM COMMENTS WHERE ID = :pid"), {'pid': response.parent_id}).fetchone()._mapping
                        if (original.watch_id != watchlist.id):
                            keep_looping = False;
                    else:
                        keep_looping = False;
                else:
                    keep_looping = False;
            else:
                keep_looping = False;

        ai_response = botAI.craft_response(history, system_prompt).choices[0].message.content
        # Record token usage
        user_usage = len(enc.encode(system_prompt + history + ai_response))
        conn.execute(text("""UPDATE USERS
                             SET TOKEN_USAGE = TOKEN_USAGE + :uu
                             WHERE ID = :id;"""), {'uu': user_usage, 'id': comment.user_id})


        match = re.search(r'^(YES|NO)', ai_response, re.MULTILINE)
        if match == None:
            # Ignore response
            logger.info("AI returned invalid response, missing YES or NO at start to comment chain \"" + history + "\"")
        elif match.group(0) == "YES":
            logger.debug("Bot accepted text\"" + comment.text + "\"")
            # delete unnecessary stuff
            ai_response = ai_response.replace("YES", "", 1)
            ai_response = re.sub(r'(\[\[\[.*?\]\]\])+', '', ai_response, re.DOTALL)
            ai_response = re.sub(r'(^\n+|$\n(\n+))', '', ai_response)
            if (ai_response == ""):
                logger.info("AI returned a YES response without creating a reply.\n")
            else:
                # send reply
                reddit_comment = reddit.comment(id = comment.reddit_id)

                try:
                    bot_reply = reddit_comment.reply(ai_response + "\n\n " + constants.BOT_DISCLAIMER)
                    conn.execute(text("""INSERT     INTO COMMENTS (REDDIT_ID, TEXT, USER_ID, WATCH_ID, IS_SUBMISSION, PARENT_ID)
                                         VALUES     (:rid, :txt, :uid, :wid, 0, :pid);"""), {
                                 'rid': bot_reply.id, 'txt': bot_reply.body, 'uid': botDB.get_id(constants.BOT_USERNAME, conn),
                                 'wid': parent.watch_id, 'pid': comment.id})
                    logger.info(f"Posted generated response \"{ai_response}\" to comment \"{comment.text}\"")
                    reply_count += 1
                except Exception as e:
                    logger.info("Creating reply failed. Error: " + str(e))

        elif match.group(0) == "NO":
            logger.debug("Bot rejected text \"" + comment.text + "\"")

        conn.execute(text("""UPDATE COMMENTS
                             SET NEED_TO_EVALUATE = 0
                             WHERE ID = :id"""), {'id': comment.id})

    conn.commit()
    logger.info(f"Finished evaluating {count} replies ({reply_count} replies generated)")

def main():
    logger = botLogger.init_logger(logging.INFO)

    reddit_client_id = os.getenv("REDDIT_CLIENT_ID")
    reddit_client_secret = os.getenv("REDDIT_SECRET")
    reddit_password = os.getenv("REDDIT_PASSWORD")

    logger.info("Starting bot")

    reddit = praw.Reddit(
        client_id = reddit_client_id,
        client_secret = reddit_client_secret,
        password = reddit_password,
        user_agent= constants.BOT_USERNAME + " (Using PRAW author u/0046)",
        username=constants.BOT_USERNAME
        )
    logger.info("User logged in: " + reddit.user.me().name)

    logger.info("Starting SQL server")
    # Connect to SQL database
    engine = botDB.init_database("sqlite:///botDB.db")

    # Create connection and enable foreign keys
    conn = engine.connect();
    conn.execute(text("pragma foreign_keys=ON"))

    # Add bot to database
    botDB.add_user(reddit.redditor(constants.BOT_USERNAME), conn)

    epoch_num = 0

    while (True):
        epoch_num += 1
        logger.info("~~~~~~~~~")
        logger.info(f"Epoch {epoch_num}")

        # Delete old watchlists. This will remove all comments and submissions in the watchlist from consideration
        cleanup_watchlists(reddit, conn, logger)

        # Read inbox for replies, mentions, and messages
        read_inbox(reddit, conn, logger)

        # Look for new replies to original submissions
        read_submission_replies(reddit, conn, logger)

        # Perform evaluations and possibly reply
        evaluate_comments(reddit, conn, logger)

        logger.info(f"Sleeping for {constants.SLEEP_TIME} seconds")
        time.sleep(constants.SLEEP_TIME)


    logger.info("Terminating bot")
    conn.close()

if __name__ == "__main__":
    main()
