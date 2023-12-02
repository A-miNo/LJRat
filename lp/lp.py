import argparse
import socket
import worker
import menu
import sys
import select
from modules import *
from modules import mod_funcs
from globals import ctx

def main():
    parser = argparse.ArgumentParser(description='LP options')
    parser.add_argument('-l', '--log', type=str, default="logs", help="Logging directory")
    parser.add_argument('ip', type=str, nargs='?', default="127.0.0.1", help="Interface to listen on")
    parser.add_argument('port', type=int, nargs='?', default=5555, help="Port to listen on")
    args = parser.parse_args()

    # Build menu by dynamnically importing all modules and assigning
    # their entrypoints as do_ functions so they are populated in the menu
    # automagically

    cmd_menu = menu.Menu()
    for module in mod_funcs:
        print(f"Loading {module}")
        setattr(type(cmd_menu), 'do_' + module, globals()[module].entrypoint)
        ctx.deserializers[globals()[module].MODULE_ID] = globals()[module]._deserialize

    ctx.log_dir = args.log

    print(f"Waitng for connection on {args.ip}:{args.port}")
    listen_sock = socket.socket(family=socket.AF_INET, type=socket.SOCK_STREAM)
    listen_sock.bind((args.ip, args.port))
    listen_sock.listen(5)

    inputs = [listen_sock]
    outputs = []
    errors = []

    while inputs:
        try:
            read, _, _ = select.select(inputs, outputs, errors, 1)
        except KeyboardInterrupt as e:
            sys.exit(-1)

        for s in read:
            conn = s.accept()
            read.remove(s)
            inputs.remove(listen_sock)
            listen_sock.close()

    # Should only be expecting one callback
    listen_sock.close()

    # Start up the worker threads
    recv_thread = worker.Recv_Worker(conn[0])
    recv_thread.daemon = True
    recv_thread.start()
    send_thread = worker.Send_Worker(conn[0])
    send_thread.daemon = True
    send_thread.start()

    try:
        cmd_menu.cmdloop()

    except KeyboardInterrupt as e:
        recv_thread.stop = True
        send_thread.stop = True
        sys.exit(-1)

if __name__ == '__main__':
    main()