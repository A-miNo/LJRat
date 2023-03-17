# module to write command output to a file
from datetime import datetime
import os


def write_to_log(log_dir, cmd_type, data):
    log = build_log_name(cmd_type)
    print(log_dir+log)
    with open (log_dir + "\\" + log + ".log", 'wb') as outfile:
        outfile.write(data)


def build_log_name(cmd_type):
    # current date and time
    now = datetime.now()

    filename = now.strftime("%m_%d_%Y_%H_%M_%S_")
    filename += cmd_type

    return filename