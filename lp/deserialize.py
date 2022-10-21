import struct
from globals import *

def process_result(data):
    header = data[:HEADER_LEN]
    payload = data[HEADER_LEN:]
    message_size, cmd_id, job_id, result_code = struct.unpack('IIII', header)

    print(f"size: {message_size}, cmd_id: {cmd_id}, job_id: {job_id}, result_code: {result_code}")

