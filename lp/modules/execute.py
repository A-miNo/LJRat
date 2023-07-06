import struct
from helper import str_to_c_str
from globals import ctx
from session import HEADER_LEN, INT_SIZE

MODULE_ID = 0x02

def entrypoint(self, args):
    """Execute a file on remote target
    'execute netstat -ant'
    """
    print("Execute")

def _deserialize(data):
    pass