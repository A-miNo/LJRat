import socket
from globals import *

class command:
    unique_id = 0

    def __init__(self, cmd_id, args_num, cmd):
        self.cmd_id = cmd_id
        self.args_num = args_num
        self.cmd = cmd
        self.unique_id = command.unique_id
        command.unique_id += 1
    
    def __str__(self):
        return f"Unique ID: {self.unique_id}\nCMD: {self.cmd}"

def add_listen_socket(ip, port, alias_name):

    sock = socket.socket(family=socket.AF_INET, type=socket.SOCK_STREAM)
    sock.bind((ip, port))
    sock.listen(5)

    host_list.add_host(ip, sock, alias_name)
    listen_socks.append(sock)
    bound_socks.append(sock)