"""
Establishes the menu class and function overloads
"""

import cmd
import time
import error
from error import *
from globals import ctx
from context import State

class Menu(cmd.Cmd):
    '''Custom implementation of the CMD class'''
    prompt = ctx.prompt

    def do_exit(self, arg):
        '''Function to exit the CMD.cmdloop by returning True'''
        return (None, E_EXIT)

    def postcmd(self, results, line):
        ''' Overrride function that conducts checks after command is executed'''

        # Command modules entry point is executed and returns 'results' which is in tuple format (msg, error_code)
        # Core modules do not send anything to the remote side so None is returned
        if None != results:   
            # Not used for anything yet
            error_code = results[1]
            
            if E_EXIT == error_code:
                return True
            if E_SUCCESS != error_code:
                print(ERROR_TABLE[error_code])
                return None

            msg = results[0]
            ctx.send_queue.put(msg)

            
            # Check to see if command that was executed has results that have already processed
            while msg.job_id not in ctx.processed and ctx.state != State.DISCONNECTED.value:
                time.sleep(1)

        # Ensure prompt gets update in case of disconnection
        self.prompt = ctx.prompt


        return None
    
    0
