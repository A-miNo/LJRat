import struct
import message
import os
import log
from error import *
from helper import str_to_c_str
from globals import ctx
from session import HEADER_LEN, INT_SIZE

MODULE_ID = 0x03

def entrypoint(self, args):
    """ Put a file onto remote target
    'put c:\\path\\to\\local_file.exe c:\\path\\to\\remote_file.exe'
    """
    
    # Validate local file exists prior to building the message
    args = args.split()
    if not os.path.exists(args[0]):
        return (None, E_FILE_EXISTS_ERROR)

    module_args = {"LOCAL_FILE": args[0],"REMOTE_FILE_NAME": args[1], "JOB_ID": ctx.get_next_job()}
    msg = message.Message(_serialize(module_args))
    ctx.send_queue.put(msg)
    return (msg, E_SUCCESS)

def _serialize(data):
    serialized_data = bytearray()
    file_bytes = None

    result_code = 0
    cmd_type = MODULE_ID
    job_id = data["JOB_ID"]

    with open(data["LOCAL_FILE"], "rb") as infile:
        file_bytes = infile.read()
    
    local_file = file_bytes
    local_file_len = len(local_file)
    remote_file = str_to_c_str(data["REMOTE_FILE_NAME"])
    remote_filename_len = len(remote_file)
    data_len = HEADER_LEN + INT_SIZE + local_file_len + INT_SIZE + remote_filename_len 
    # TODO Look to remove RemoteFileName len because it is null terminated already
    serialized_data.extend(struct.pack("I", data_len))
    serialized_data.extend(struct.pack("I", job_id))
    serialized_data.extend(struct.pack("I", cmd_type))
    serialized_data.extend(struct.pack("I", result_code))
    serialized_data.extend(struct.pack("I", local_file_len))
    serialized_data.extend(struct.pack(f"{local_file_len}s", local_file))
    serialized_data.extend(struct.pack("I", remote_filename_len))
    serialized_data.extend(struct.pack(f"{remote_filename_len}s", remote_file))

    return serialized_data


def _deserialize(msg):
    log_dir = ctx.log_dir

    if msg.result_code == E_SUCCESS:
        log.write_to_log(log_dir, ".put", ERROR_TABLE[msg.result_code], "File sucessfully uploaded")
    else:
        print(f"Error uploading file: {ERROR_TABLE[msg.result_code]}")

    return