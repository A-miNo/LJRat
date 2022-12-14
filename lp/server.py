from symbol import continue_stmt
import threading
from time import sleep
from xmlrpc.client import Server
import log
import worker
import os
import socket
import select
import queue
import struct
from globals import *
from serialize import serialize
from deserialize import process_result

class ServerThread(threading.Thread):
    
    def __init__(self, logging_dir):
        super().__init__(target=self.server_handler)
        self.should_stop = False
        self.log_name = os.path.join(logging_dir, "server")
        os.makedirs(self.log_name, exist_ok=True)
        self.log = log.Log(self.log_name)

    def server_handler(self):
        # Wait for the user to tell us to listen

        while self.should_stop != True:
            # Loop until the user specified a listener

            while (len(bound_socks) == 0 and len(listen_socks) == 0 and self.should_stop != True):
                sleep(1)
            
            try:
                listen_ready, send_ready, except_ready = select.select(listen_socks, send_socks, [], 5.0)
            except OSError as e:
                # Cntl-C before a listener
                if self.should_stop == True:
                    break
            
            # Iterate through all of the sockets that have data waiting
            for sock in listen_ready:
                alias = host_list.get_alias(sock)
                host = host_list.hosts[alias]
                
                # If socket has data waiting and is one of the bound sockets, accept connection
                if sock in bound_socks:
                    (client_socket, host_info) = sock.accept()
                    #print(f"Connection from {host_info}")

                    host_list.hosts[alias].client_sock = client_socket
                    host_list.hosts[alias].status = CONNECTED

                    # Add our new client socket to select lists to watch for data
                    listen_socks.append(client_socket)

                    # Stop listening on bound socket, should only need to catch one callback
                    sock.close()
                    bound_socks.remove(sock)
                    listen_socks.remove(sock)
                else:
                    data = recv_data(sock)
                    if not data:
                        host.status = DISCONNECTED
                    else:
                        process_result(data)
                    listen_socks.remove(sock)

            for sock in send_ready:
                alias = host_list.get_alias(sock)
                host = host_list.hosts[alias]

                #Might want to move serialization to main thread for speed down the road
                while len(host.send_queue):
                    job = host.send_queue.pop()
                    data = serialize(job)
                    send_data(host.client_sock, data)

                send_socks.remove(sock)


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
