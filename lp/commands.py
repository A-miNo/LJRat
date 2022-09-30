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

# Command values are CMD_ID and ARGS_NUM
commands = {"get":[0, 1], "put":[1, 1], "execute":[2, 1], "module":[3, 1], "close_session":[4, 0]}

def parse_command(user_input):
    cmd = user_input.split()
    cmd_data = commands.get(cmd[0], None)

    # Ensure we have a valid command
    if not cmd_data:
        return None

    # Ensure we have a valid number of arguments
    if cmd_data[1] != len(cmd) - 1:
        return None

    return command(cmd_data[0], cmd_data[1], cmd)

def add_listen_socket(ip, port, alias_name):

    sock = socket.socket(family=socket.AF_INET, type=socket.SOCK_STREAM)
    sock.bind((ip, port))
    sock.listen(5)

    host_list.add_host(ip, sock, alias_name)
    listen_socks.append(sock)
    bound_socks.append(sock)