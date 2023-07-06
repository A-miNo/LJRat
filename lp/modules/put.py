import struct
from helper import str_to_c_str
from globals import ctx
from session import HEADER_LEN, INT_SIZE

MODULE_ID = 0x03

def entrypoint(self, args):
    """ Put a file onto remote target
    'put c:\\path\\to\\local_file.exe c:\\path\\to\\remote_file.exe'
    """
    
    print("Put")

def _deserialize(data):
    pass