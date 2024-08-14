"""
LJRat LP core
"""

import argparse
import menu
import sys
from modules import * # Used to kick off __all__ in init.py to populate available modules
from modules import mod_funcs
from core import * # Used to kick off __all__ in init.py to populate available core modules
from core import core_funcs
from globals import ctx

def main():
    parser = argparse.ArgumentParser(description='LP options')
    parser.add_argument('-l', '--log', type=str, default="logs", help="Logging directory")
    parser.add_argument('ip', type=str, nargs='?', default="127.0.0.1", help="Interface to listen on")
    parser.add_argument('port', type=int, nargs='?', default=5555, help="Port to listen on")
    args = parser.parse_args()

    print('''

        
 __           __     ______     ______     ______  
/\ \         /\ \   /\  == \   /\  __ \   /\__  _\ 
\ \ \____   _\_\ \  \ \  __<   \ \  __ \  \/_/\ \/ 
 \ \_____\ /\_____\  \ \_\ \_\  \ \_\ \_\    \ \_\ 
  \/_____/ \/_____/   \/_/ /_/   \/_/\/_/     \/_/ 
                                                   

    ''')

    cmd_menu = menu.Menu()

    # Import all the builtins
    for module in core_funcs:
        print(f"Loading {module}")
        setattr(cmd_menu, 'do_' + module, globals()[module].entrypoint)

    # Build menu by dynamnically importing all modules and assigning
    # their entrypoints as do_ functions so they are populated in the menu
    # automagically
    for module in mod_funcs:
        print(f"Loading {module}")
        setattr(type(cmd_menu), 'do_' + module, globals()[module].entrypoint)
        ctx.deserializers[globals()[module].MODULE_ID] = globals()[module]._deserialize


    ctx.log_dir = args.log

    try:
        cmd_menu.cmdloop()
    except KeyboardInterrupt as e:
        sys.exit()

if __name__ == '__main__':
    main()