import host

host_list = host.Host_Container()
bound_socks = []
listen_socks = []
send_socks = []

server_obj = None

HEADER_LEN = 16
INT_SIZE = 4
DISCONNECTED = 2
CONNECTED = 1
