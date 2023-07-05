import cmd
from globals import *

class Menu(cmd.Cmd):
    intro = "LJRat"
    prompt = "> "
    current_host = None

    def do_exit(self, arg):
        '''Function to exit the CMD.cmdloop by returning True'''
        return True
