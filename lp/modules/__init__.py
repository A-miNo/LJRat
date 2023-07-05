from os.path import dirname, basename, isfile, join
import glob

# Build a dynamic list of modules to provide an easy way of extending the functionality of LJRat
module_files = glob.glob(join(dirname(__file__), "*.py"))
mod_funcs = [ basename(f)[:-3] for f in module_files if isfile(f) and not f.endswith('__init__.py')]
__all__ = mod_funcs