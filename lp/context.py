"""
Module that establishes a context that is used throughout the operation of LJRat
"""

import queue
import threading
import time
from enum import Enum

class State(Enum):
    DISCONNECTED = 0
    CONNECTED = 1


class Rat_Ctx():
    '''Class that maintains the current state of the RAT'''

    def __init__(self):
        '''Initialize all the attributes'''
        self.job_id = 0
        self.send_queue = queue.Queue()
        self.recv_queue = queue.Queue()
        self.conn = None
        self.processed = []
        self.lock = threading.Lock()
        self.loaded_modules = {}
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
        '''Internal fucntion that increments job-id'''
        self.job_id += 1
        return self.job_id
    
    def update_state(self, state):
        ''' Updates the prompt with current state of the connection'''
        self.state = state
        self._update_prompt()

    def _update_prompt(self):
        '''Internal function that Updates the prompt with current state of the connection'''
        if self.state == State.DISCONNECTED.value:
            self.prompt = "DISCONNECTED > "
        
        else:
            self.prompt = f"{self.target_name} > "

    def check_loaded(self, module_id):
        return self.loaded_modules.get(module_id)["loaded"]
    
    def get_module_name(self, module_id):
        return self.loaded_modules.get(module_id)["dll_name"]
    
    def get_module_id (self, module_name):
        module_id = None
        for k, v in self.loaded_modules.items():
            if v.get('dll_name') == module_name:
                module_id = k
        return module_id
    
    def update_loaded(self, module_id):
        self.loaded_modules[module_id]["loaded"] = True
        return
    
    def queue_work(self, msg):
        self.send_queue.put(msg)

    def queue_work_and_wait(self, msg):
        self.send_queue.put(msg)

        while (msg.job_id not in self.processed and self.state != State.DISCONNECTED.value):
            time.sleep(1)

