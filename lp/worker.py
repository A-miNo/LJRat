import threading
import network

class Worker(threading.Thread):
    def __init__(self, conn):
        super().__init__(target=self.Worker)
        self.conn = conn
        self.stop = False

    def Worker(self):
        while not self.stop:
            try:
                data = network.recv_data(self.conn)
            except ConnectionError as e:
                print(e)
                self.stop = True