import argparse
import socket
import threading
import time
import sys
import commands
from server import ServerThread
import menu

def main():
    parser = argparse.ArgumentParser(description='LP options')
    parser.add_argument('-logging_dir', type=str, default="logs", help="Logging directory")
    args = parser.parse_args()

    server_thread = ServerThread(args.logging_dir)
    server_obj = server_thread
    server_thread.start()

    menu_loop = menu.Menu()

    try:
        menu_loop.cmdloop()

    except KeyboardInterrupt as e:
        pass
    
    server_thread.should_stop = True
  



if __name__ == '__main__':
    main()