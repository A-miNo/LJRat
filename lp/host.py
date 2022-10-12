import queue
import globals

class Host_Container():
    '''Class used to maintain a list of hosts as well as provide additional helper funtionality'''

    def __init__(self):
        self.hosts = {}

    def add_host(self, ip, bound_sock, alias_name):
        '''Creates a host object and adds it to the hosts dictionary'''

        self.hosts[alias_name] = Host(bound_sock, alias_name)

    def get_alias(self, sock):
        '''Function used to derive the target alias name from the socket assigned to the connection'''

        for alias_name in self.hosts:
            host = self.hosts[alias_name]
            if host.bound_sock == sock or host.client_sock == sock:
                return alias_name



class Host():
    '''Class used to maintain connection information and manage taskings per target'''
    
    host_status_enum = {0:"Not connected", 1:"Connected", 2:"Disconnected"}

    def __init__(self, bound_sock, alias_name):
        self.send_queue = []
        self.recv_queue = []
        self.bound_sock = bound_sock
        self.alias_name = alias_name
        self.client_sock = None
        self.job_id = 1
        self.status = 0 # TODO better way to go about handling this
        
    def add_job(self, data):
        '''Function used to add a job to the host send queue'''

        data["JOB_ID"] = self.job_id
        self.job_id += 1
        self.send_queue.append(data)
        if self.client_sock:
            globals.send_socks.append(self.client_sock)

    def __repr__(self):
        return f"Alias: {self.alias_name}, listening on {self.bound_sock}, connected on {self.client_sock}"