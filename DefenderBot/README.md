## Defender Bot

A Reddit bot to provide rhetorical defense for users. Developed using PRAW, tiktoken, SQLAlchemy, and OpenAI.

### Requirements to run
- Install Python modules

```
pip3 install praw tiktoken sqlalchemy openai
```

- Set environmental variables

```
echo "export OPENAI_API_KEY='yourkey' REDDIT_CLIENT_ID='botid' REDDIT_SECRET='botsecret' REDDIT_PASSWORD='botpassword'" >> ~/.bash_profile && source ~/.bash_profile
```

- Run

```
python3 main.py
```
