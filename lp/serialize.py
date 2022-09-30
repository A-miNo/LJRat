import struct

cmd_lookup = {"GET": 1, "PUT": 2, "EXECUTE": 3}
HEADER_LEN = 16

def serialize(data):
    serialize_funcs = {"GET": _serialize_get}

    func = serialize_funcs.get(data[0])
    return func(data)

def _serialize_get(data):
    send_data = bytearray()

    cmd_type = cmd_lookup.get(data[0])
    job_id = data[2]
    file_name_len = len(data[1])
    data_len = HEADER_LEN + file_name_len

    send_data.extend(struct.pack("I", data_len))
    send_data.extend(struct.pack("I", cmd_type))
    send_data.extend(struct.pack("I", job_id))
    send_data.extend(struct.pack(f"{file_name_len}s", data[1][0].encode()))

    return send_data

def _serialize_put(data):
    pass

def _serialize_execute(data):
    pass