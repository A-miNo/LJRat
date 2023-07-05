import struct
from globals import *

def recv_data(sock):
    try:
        data = sock.recv(16)
    except ConnectionResetError as e:
        return None

    header = struct.unpack('IIII', data)
    payload = sock.recv(header[0] - HEADER_LEN)
    data += payload
    
    return data

def send_data(sock, data):
    return sock.send(data)