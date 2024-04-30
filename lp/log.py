"""
Module to write command output to a file
"""
 
from datetime import datetime
import os

def write_to_log(log_dir, log_suffix, data, msg_txt):
    if not os.path.exists(log_dir):
        os.mkdir(log_dir)
    log_name = build_log_name(log_suffix)
    log_file = log_dir + os.sep + log_name
    print(f"{msg_txt} {log_file}")

    if isinstance(data, bytes):
        with open(log_file, 'wb') as outfile:
            outfile.write(data)
    else:
        with open(log_file, 'w') as outfile:
            outfile.write(data)



def build_log_name(log_suffix):
    # current date and time
    now = datetime.now()
    filename = now.strftime("%m_%d_%Y_%H_%M_%S_")

    return filename + log_suffix