import struct
from globals import *

# HEADER (16 bytes) + Data (N Bytes)
# -----------------------------------------------------------------------------#
#       4 Bytes       | 4 Bytes |  4 Bytes  |   4 Bytes   | N Bytes            #
#  Length of result   | Job-ID  | Module ID | Result Code |  Data              #
# -----------------------------------------------------------------------------#

HEADER_LEN = 16
INT_SIZE = 4

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