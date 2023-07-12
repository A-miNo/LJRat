import threading
import session
import message
from globals import ctx

class Recv_Worker(threading.Thread):
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

            msg = message.Message(data)
            msg.deserializer = ctx.deserializers[msg.module_id]
            print(msg)
            # Find the appropriate deserializer and process the message


class Send_Worker(threading.Thread):
    def __init__(self, conn):
        super().__init__(target=self.Worker)
        self.stop = False
        self.conn = conn

    def Worker(self):
        while ctx.send_queue:
            msg = ctx.send_queue.get()
            session.send_data(self.conn, msg.data)
