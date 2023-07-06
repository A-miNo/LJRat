import threading
import session
from globals import ctx

class Worker(threading.Thread):
    def __init__(self, conn):
        super().__init__(target=self.Worker)
        self.conn = conn
        self.stop = False

    def Worker(self):
        while not self.stop:
            # Lets get one message at a time
            try:
                data = session.recv_data(self.conn)
            except ConnectionError as e:
                print(e)
                self.stop = True

            # Find the appropriate deserializer and process the message
