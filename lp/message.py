"""
Module to write command output to a file
"""

import struct
from session import HEADER_LEN

class Message():
    '''Class that captures the header and data for a message for use by the LP'''
    def __init__(self, data):
        '''Initialize all the attributes'''
        self.data = data
        self._process()
        self.deserializer = None
    
    def _process(self):
        '''Function that unpacks a messages header information for use in menu post_cmd'''
        self.header = struct.unpack('IIII', self.data[0:16])
        self.size = self.header[0]
        self.job_id = self.header[1]
        self.module_id = self.header[2]
        self.result_code = self.header[3]

        if len(self.data) > HEADER_LEN:
            self.payload = self.data[16:]

    def __str__(self):
        return str(f"Packet size: {self.size}, Job_id {self.job_id}, "
            f"Module_id: {self.module_id}, result_code: {self.result_code}")