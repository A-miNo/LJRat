import threading
import session
import message
import queue
from globals import ctx
from context import State

class Recv_Worker(threading.Thread):
    '''Thread that waits for data to be sent to process'''
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
            # Add processed job to list of completed jobs so menu can continue
            ctx.processed.append(msg.job_id)


class Send_Worker(threading.Thread):
    '''Function to iterate through pending jobs to send to remote side'''
    def __init__(self):
        super().__init__(target=self.Worker)

    def Worker(self):
        while State.DISCONNECTED.value != ctx.state:
            try:
                msg = ctx.send_queue.get(True, 1)
            except queue.Empty:
                continue

            session.send_data(ctx.conn, msg.data)
