import queue
import threading

class Rat_Ctx():
    '''Class that maintains the current state of the RAT'''

    def __init__(self):
        self.job_id = 0
        self.send_queue = queue.Queue()
        self.recv_queue = queue.Queue()
        self.lock = threading.Lock()
        self.loaded_modules = []
        self.deserializers = {}
        self.log_dir = "logs"
    
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