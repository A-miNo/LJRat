import struct
from globals import *
import log

def process_get(log_dir, data):
    log.write_to_log(log_dir, "GET", data)

def process_put(data):
    pass

def process_result(log_dir, data):
    header = data[:HEADER_LEN]
    payload = data[HEADER_LEN:]
    message_size, cmd_id, job_id, result_code = struct.unpack('IIII', header)

    if cmd_id == 0:
        process_get(log_dir, payload)
    
    print(f"size: {message_size}, cmd_id: {cmd_id}, job_id: {job_id}, result_code: {result_code}")



