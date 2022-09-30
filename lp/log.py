# module to write command output to a file
from datetime import datetime
import os

class Log():

    def __init__(self, logging_dir):
        self.logging_dir = os.getcwd() + "\\" + logging_dir + "\\"
        try:
            os.mkdir(self.logging_dir)
        except FileExistsError as e:
            print("Logging directory already exists, using existing directory")
            pass

    def write_to_log(self, cmd_type, data):
        log = self.build_log_name(cmd_type)
        with open (self.logging_dir + log, 'w') as outfile:
            outfile.write(data)


    def build_log_name(self, cmd_type):
        # current date and time
        now = datetime.now()

        filename = now.strftime("%m_%d_%Y_%H_%M_%S_")
        filename += cmd_type

        return filename