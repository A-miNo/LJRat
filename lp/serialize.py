import struct
from globals import *

def serialize(data):
    serialize_funcs = {"GET": _serialize_get, "PUT": _serialize_put, "EXECUTE": _serialize_execute}

    func = serialize_funcs[data["CMD_TYPE"]]
    return func(data)

def _serialize_get(data):
    serialized_data = bytearray()

    result_code = 0
    cmd_type = cmd_lookup.get(data["CMD_TYPE"])
    job_id = data["JOB_ID"]
    remote_filename_len = len(data["REMOTE_FILE"])
    data_len = HEADER_LEN + remote_filename_len + INT_SIZE

    serialized_data.extend(struct.pack("I", data_len))
    serialized_data.extend(struct.pack("I", cmd_type))
    serialized_data.extend(struct.pack("I", job_id))
    serialized_data.extend(struct.pack("I", result_code))
    serialized_data.extend(struct.pack("I", remote_filename_len))
    serialized_data.extend(struct.pack(f"{remote_filename_len}s", data["REMOTE_FILE"]))

    return serialized_data

def _serialize_put(data):
    serialized_data = bytearray()

    cmd_type = cmd_lookup.get(data["CMD_TYPE"])
    job_id = data["JOB_ID"]
    remote_filename_len = len(data["REMOTE_FILE"])
    local_filename_len = len(data["LOCAL_FILE"])
    data_len = HEADER_LEN + remote_filename_len + local_filename_len

    serialized_data.extend(struct.pack("I", data_len))
    serialized_data.extend(struct.pack("I", cmd_type))
    serialized_data.extend(struct.pack("I", job_id))
    serialized_data.extend(struct.pack(f"{remote_filename_len}s", data["REMOTE_FILE"]))
    serialized_data.extend(struct.pack(f"{local_filename_len}s", data["LOCAL_FILE"]))

    return serialized_data

def _serialize_execute(data):
    serialized_data = bytearray()

    cmd_type = cmd_lookup.get(data["CMD_TYPE"])
    job_id = data["JOB_ID"]
    execute_cmd_len = len(data["EXECUTE_CMD"])
    data_len = HEADER_LEN + execute_cmd_len

    serialized_data.extend(struct.pack("I", data_len))
    serialized_data.extend(struct.pack("I", cmd_type))
    serialized_data.extend(struct.pack("I", job_id))
    serialized_data.extend(struct.pack(f"{execute_cmd_len}s", data["EXCUTE_CMD"]))

    return serialized_data