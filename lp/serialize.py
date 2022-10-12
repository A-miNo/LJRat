import struct

cmd_lookup = {"GET": 1, "PUT": 2, "EXECUTE": 3}
HEADER_LEN = 16

def serialize(data):
    serialize_funcs = {"GET": _serialize_get, "PUT": _serialize_put, "EXECUTE": _serialize_execute}

    func = serialize_funcs[data["CMD_TYPE"]]
    return func(data)

def _serialize_get(data):
    serialized_data = bytearray()

    cmd_type = cmd_lookup.get(data["CMD_TYPE"])
    job_id = data["JOB_ID"]
    remote_filename_len = len(data["REMOTE_FILE"])
    data_len = HEADER_LEN + remote_filename_len

    serialized_data.extend(struct.pack("I", data_len))
    serialized_data.extend(struct.pack("I", cmd_type))
    serialized_data.extend(struct.pack("I", job_id))
    serialized_data.extend(struct.pack(f"{remote_filename_len}s", data["REMOTE_FILE"].encode()))

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
    serialized_data.extend(struct.pack(f"{remote_filename_len}s", data["REMOTE_FILE"].encode()))
    serialized_data.extend(struct.pack(f"{local_filename_len}s", data["LOCAL_FILE"].encode()))

    return serialized_data

def _serialize_execute(data):
    serialized_data = bytearray()

    cmd_type = cmd_lookup.get(data["CMD_TYPE"])
    job_id = data["JOB_ID"]
    execute_cmd_len = len(data["EXECUTE_CMD"])
    data_len = HEADER_LEN + file_name_len

    serialized_data.extend(struct.pack("I", data_len))
    serialized_data.extend(struct.pack("I", cmd_type))
    serialized_data.extend(struct.pack("I", job_id))
    serialized_data.extend(struct.pack(f"{execute_cmd_len}s", data["EXCUTE_CMD"].encode()))

    return serialized_data