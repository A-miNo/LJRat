"""
Establishes the menu class and function overloads
"""

import cmd
import time
import error
import os
import modules.load_module
from error import *
from globals import ctx, MODULE_PATH
from context import State

EXIT_VALUE = 0x1

class Menu(cmd.Cmd):
    '''Custom implementation of the CMD class'''
    prompt = ctx.prompt

    def do_exit(self, arg):
        '''Function to exit the CMD.cmdloop by returning True'''
        return EXIT_VALUE

    def postcmd(self, msg, line):
        ''' Overrride function that conducts checks after command is executed'''
        if msg == EXIT_VALUE:
            return True
        
        # Check to see if the command being executed depends on a module that needs to be loaded
        # More of a nice to have, instead of having to load modules individually
        if None != msg:
            if not ctx.check_loaded(msg.module_id):
                # Logic to load library
                module_name = ctx.get_module_name(msg.module_id)
                load_msg = modules.load_module.entrypoint(ctx, module_name)
                print(f"Loading {module_name}")
                ctx.queue_work_and_wait(load_msg)
                ctx.update_loaded(msg.module_id)

            ctx.queue_work_and_wait(msg)
            # If a module is loaded independently, we need to be aware and update list
            if msg.remote_module_id:
                ctx.update_loaded(msg.remote_module_id)

        # Ensure prompt gets update in case of disconnection
        self.prompt = ctx.prompt


        return None