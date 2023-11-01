import logging.handlers
import os

g_logger = None

def init_logger(level=None):
    global g_logger
    g_logger = logging.getLogger("bot_logger")
    if level == None:
        level = logging.INFO
    g_logger.setLevel(level)

    # Setup stream
    log_formatter = logging.Formatter('%(asctime)s - %(levelname)s: %(message)s')
    log_stream_handler = logging.StreamHandler()
    log_stream_handler.setFormatter(log_formatter)
    g_logger.addHandler(log_stream_handler)

    # Setup log files - 2 MB each and store 30
    log_file_handler = logging.handlers.RotatingFileHandler("logs/bot.log", maxBytes=1024 * 1024 * 2, backupCount=32)
    log_file_handler.setFormatter(log_formatter)
    g_logger.addHandler(log_file_handler)

    g_logger.info("Started logger")
    return g_logger

def get_logger():
    if (g_logger == None):
        raise ValueError("Logger is not initialized!")
    else:
        return g_logger
