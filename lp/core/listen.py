from globals import ctx
from context import State
from error import SYNTAX_TABLE, E_LISTEN_ERROR
import socket
import sys
import select
import worker

def entrypoint(self, args):
    args = args.split()

    args = validator(args)
    if not args:
        return None
    

    print(f"Waitng for connection on {args[1]}:{args[2]}")
    listen_sock = socket.socket(family=socket.AF_INET, type=socket.SOCK_STREAM)
    listen_sock.bind((args[1], int(args[2])))
    listen_sock.listen(5)

    inputs = [listen_sock]
    outputs = []
    errors = []

    while inputs:
        read, _, _ = select.select(inputs, outputs, errors, 1)

        for s in read:
            ctx.conn = s.accept()[0]
            read.remove(s)
            inputs.remove(listen_sock)
            listen_sock.close()

    # Should only be expecting one callback
    listen_sock.close()
    ctx.target_name = args[0]
    ctx.update_state(State.CONNECTED.value)

    # Start up the worker threads
    recv_thread = worker.Recv_Worker()
    recv_thread.daemon = True
    recv_thread.start()
    send_thread = worker.Send_Worker()
    send_thread.daemon = True
    send_thread.start()


    return

def validator(args):
    args = args
    if len(args) != 3:
        print(SYNTAX_TABLE[E_LISTEN_ERROR])
        args = None
    
    return args