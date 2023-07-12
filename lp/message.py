import struct
from session import HEADER_LEN

class Message():
    def __init__(self, data):
        self.data = data
        self.process()
        self.deserializer = None
    
    def process(self):
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