import threading
import session
import message
import queue
from globals import ctx
from context import State

class Recv_Worker(threading.Thread):
    def __init__(self):
        super().__init__(target=self.Worker)

    def Worker(self):
        while State.DISCONNECTED.value != ctx.state:
            # Lets get one message at a time
            try:
                data = session.recv_data(ctx.conn)
            except ConnectionError as e:
                print(e)
            
            if None == data:
                self.stop = True
                ctx.update_state(State.DISCONNECTED.value)
                continue

            msg = message.Message(data)
            deserializer = ctx.deserializers[msg.module_id]

            deserializer(msg)
            # Add processed job to list for backgrounding commands
            ctx.processed.append(msg.job_id)

            #print(msg)
            # Find the appropriate deserializer and process the message


class Send_Worker(threading.Thread):
    def __init__(self):
        super().__init__(target=self.Worker)

    def Worker(self):
        while State.DISCONNECTED.value != ctx.state:
            try:
                msg = ctx.send_queue.get(True, 1)
            except queue.Empty:
                continue

            session.send_data(ctx.conn, msg.data)
