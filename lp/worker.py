import threading
import struct
import binascii
import log

class WorkerThread(threading.Thread):
    
    def __init__(self, sock, logging_dir):
        super().__init__(target=self.client_recv_handler, args=(sock,))
        self.stop = False
        self.sock = sock
        self.log = log.Log(logging_dir)

    def client_recv_handler(self, sock):
        while self.stop != True:
            data_tup = self.recv_data(sock)
            self.process_data(data_tup)
    
    def recv_data(self, sock):
        data = sock.recv(16)
        header = struct.unpack('IIII', data)
        print(header)
        payload = sock.recv(header[0])
        #print(payload.decode('utf-16le'))
        data += payload
        
        return (header, payload)
    
    def process_data(self, data_tup):
        # Dict with command types
        self.log.write_to_log("test", data_tup[1].decode('utf-16le'))
        print(data_tup[1].decode('utf-16le'))

