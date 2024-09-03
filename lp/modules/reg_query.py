"""
Module that adds the put functionality to LJRat
"""

import struct
import message
import os
import log
import binascii
from error import *
from helper import str_to_c_str, get_timestamp_str, filetime_to_dt, sizeof_fmt
from globals import ctx
from session import HEADER_LEN, INT_SIZE

MODULE_ID = 0x07
LOADABLE = True
DLL_NAME = "registry.dll"
PARENT = "registry"

reg_types = {0:"REG_NONE", 1:"REG_SZ", 2:"REG_EXPAND_SZ", \
                 3:"REG_BINARY", 4:"REG_DWORD", 5:"REG_DWORD_BIG_ENDIAN", \
                 6:"REG_LINK", 7:"REG_MULTI_SZ", 8:"REG_RESOURCE_LIST", \
                 9:"REG_FULL_RESOURCE_DESCRIPTOR", 10:"REG_RESOURCE_REQUIREMENTS_LIST", 11:"REG_QORD"}

def entrypoint(self, args):
    """ Query the registry
    'reg_query HKLM\Software'
    Valid Hives are HKLM, HKCR, HKCU, HKU, HKCC
    """
    
    # Validate local file exists prior to building the message
    args = validator(args)
    if not args:
        return None

    module_args = {"HIVE": args[0], "KEY": args[1], "JOB_ID": ctx.get_next_job()}
    msg = message.Message(_serialize(module_args))
    return (msg, E_SUCCESS)


def validator(args):
    '''Nothing checked at this time'''
    hive_list = {"HKLM": 0x80000002, "HKCR": 0x80000000, "HKCU": 0x80000001, "HKU": 0x80000003, "HKCC": 0x80000005}
    args = args.split('\\', maxsplit=1)
    reg_args = []

    # Check if module is loaded
    if not ctx.check_loaded(DLL_NAME):
        print("Module not loaded - 'load_module registry'")
        return None

    # Ensure we are getting a fully formed command
    if len(args) == 0 or len(args) > 2:
        print("Invalid command - Refer to help for valid syntax")
        return None
    
    # Make sure we are getting hives we know of
    hive = hive_list.get(args[0].upper(), None)
    if not hive:
        print("Invalid Hive)")
        return None
    else:
        reg_args.append(hive)

    # if subkey is not provided, we are going to send a null string to RegOpenKeyEx
    if len(args) == 1:
        reg_args.append('\0\0\0\0')
    else:
        reg_args.append(args[1])

    return reg_args

def _serialize(data):
    '''Function to take all the required arguments and pack a data structure with binary data'''
    serialized_data = bytearray()
    file_bytes = None

    result_code = 0
    cmd_type = MODULE_ID
    job_id = data["JOB_ID"]
    hive = data["HIVE"]
    reg_command_id = 1
    sub_key = str_to_c_str(data["KEY"])
    sub_key_len = len(sub_key)

    data_len = HEADER_LEN + INT_SIZE + INT_SIZE + INT_SIZE + sub_key_len
    serialized_data.extend(struct.pack("I", data_len))
    serialized_data.extend(struct.pack("I", job_id))
    serialized_data.extend(struct.pack("I", cmd_type))
    serialized_data.extend(struct.pack("I", result_code))
    serialized_data.extend(struct.pack("I", reg_command_id))
    serialized_data.extend(struct.pack("I", hive))
    serialized_data.extend(struct.pack("I", sub_key_len))
    serialized_data.extend(struct.pack(f"{sub_key_len}s", sub_key))

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

def formatter(data):
    '''Function that knows how the data will be returned from the remote side and
    turns it into a readable format'''



    subkey_count = struct.unpack('I', data[:4])[0]
    value_count = struct.unpack('I', data[4:8])[0]
    next_val = 8 # size of reg header

    for i in range(subkey_count):
        subkey_len = struct.unpack('I', data[next_val:next_val+4])[0]
        next_val += 4
        subkey_name = struct.unpack(f'{subkey_len}s', data[next_val:next_val + subkey_len])[0]
        next_val = next_val + subkey_len
        print(subkey_name.decode('ascii'))
    
    print("\n### Values ###")
    for i in range(value_count):
        value_name_len = struct.unpack('I', data[next_val:next_val+4])[0]
        next_val += 4
        value_name = struct.unpack(f'{value_name_len}s', data[next_val:next_val + value_name_len])[0]
        next_val += value_name_len
        value_type = struct.unpack('I', data[next_val:next_val+4])[0]
        next_val += 4
        value_len = struct.unpack('I', data[next_val:next_val+4])[0]
        next_val += 4
        #value_data = struct.unpack(f'{value_len}s', data[next_val:next_val + value_len])[0]
        value_data = value_formatter(value_type, data[next_val:next_val + value_len], value_len)
        next_val += value_len
        print(value_name.decode('ascii'), reg_types.get(value_type, "Unknown"), value_data, sep='\t\t')
    print()

    return

def value_formatter(data_type, value, value_len):
    ret_string = None
    
    if data_type == 0:
        ret_string = "Data type not defined"
    elif data_type == 1:
        ret_string =  struct.unpack(f'{value_len}s', value)[0].decode('ascii')
    elif data_type == 2:
        ret_string = struct.unpack(f'{value_len}s', value)[0].decode('ascii')
    elif data_type == 3:
        ret_string = f'0x{value.hex()}'
    elif data_type == 4:
        ret_string = struct.unpack(f'I', value)[0]
    elif data_type == 5:
        ret_string = struct.unpack(f'I', value)[0]
    elif data_type == 7:
        ret_string = [x.decode('ascii') for x in value.split(b'\0')[:-2]]
    elif data_type == 11:
        ret_string = struct.unpack(f'q', value)[0]

    return ret_string