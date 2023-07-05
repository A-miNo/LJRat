import struct
MODULE_ID = 0x01

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

def _deserialize_get(data):
    pass