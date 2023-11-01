from typing import List
from sqlalchemy.orm import Mapped
from sqlalchemy.orm import mapped_column
from sqlalchemy.orm import relationship
from sqlalchemy.orm import DeclarativeBase
from sqlalchemy import create_engine
from sqlalchemy import func
from sqlalchemy import String, ForeignKey, Boolean, UniqueConstraint, DateTime, Integer, select, text

g_engine = None

class Base(DeclarativeBase):
    pass

class Users(Base):
    __tablename__ = "users"

    id: Mapped[int] = mapped_column(primary_key=True)
    reddit_id: Mapped[str] = mapped_column(String)
    username: Mapped[str] = mapped_column(String)
    karma: Mapped[int] = mapped_column(Integer)
    API_key: Mapped[str] = mapped_column(String, nullable = True) # The user's openai key if it was provided
    token_usage: Mapped[int] = mapped_column(Integer, server_default="0")
    time_used: Mapped[DateTime] = mapped_column(DateTime(timezone=True), nullable = True)

    time_added: Mapped[DateTime] = mapped_column(DateTime(timezone=True), server_default=func.now())

    __table_args__ = (UniqueConstraint("username"),)

class Comments(Base):
    __tablename__ = "comments"

    id: Mapped[int] = mapped_column(primary_key=True)
    parent_id: Mapped[int] = mapped_column(Integer, nullable = True)
    reddit_id: Mapped[String] = mapped_column(String)
    text: Mapped[String] = mapped_column(String)

    user_id: Mapped[int] = mapped_column(ForeignKey("users.id"))

    watch_id: Mapped[int] = mapped_column(ForeignKey("watchlist.id", ondelete='SET NULL'), nullable=True)

    need_to_evaluate: Mapped[Boolean] = mapped_column(Boolean, server_default="0")
    is_submission: Mapped[Boolean] = mapped_column(Boolean)

    time_added: Mapped[DateTime] = mapped_column(DateTime(timezone=True), server_default=func.now())

    __table_args__ = (UniqueConstraint("reddit_id"),)

class Messages(Base):
    __tablename__ = "messages"

    id : Mapped[int] = mapped_column(primary_key=True)
    message_id: Mapped[String] = mapped_column(String)
    text: Mapped[String] = mapped_column(String)

    user_id: Mapped[int] = mapped_column(ForeignKey("users.id"))

    time_added: Mapped[DateTime] = mapped_column(DateTime(timezone=True), server_default=func.now())

    __table_args__ = (UniqueConstraint("message_id"),)

class Watchlist(Base):
    __tablename__ = "watchlist"

    id : Mapped[int] = mapped_column(primary_key=True)
    prompt: Mapped[String] = mapped_column(String)
    user_id: Mapped[int] = mapped_column(ForeignKey("users.id"))

    time_added: Mapped[DateTime] = mapped_column(DateTime(timezone=True), server_default=func.now())

def init_database(file):
    global g_engine
    g_engine = create_engine(file)

    Base.metadata.create_all(g_engine)

    return g_engine

def get_engine():
    if (g_engine == None):
        raise ValueError("Engine is not initialized!")
    else:
        return g_engine

def add_user(author, connection):
    result = connection.execute(text("""INSERT
                                        OR
                                        IGNORE     INTO USERS (REDDIT_ID, USERNAME, KARMA)
                                        VALUES     (:rid, :un, :k);"""),
                                {'rid': author.id, 'un': author.name, 'k': author.comment_karma})

    return result

def comment_in_watchlist(thread_id, connection):
    result = connection.execute(text("""SELECT    WATCH_ID
                                        FROM      COMMENTS
                                        WHERE     REDDIT_ID = :rid;"""), {'rid': thread_id}).all()
    if result and result[0] != (None,):
        return True;
    else:
        return False;

def get_id(username, connection):
    result = connection.execute(text("""SELECT  ID
                                        FROM    USERS
                                        WHERE   USERNAME = :un"""), {'un': username})
    return result.fetchone()[0]
