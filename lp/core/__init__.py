from os.path import dirname, basename, isfile, join
import glob

# Build a dynamic list of modules to provide an easy way of extending the functionality of LJRat
core_files = glob.glob(join(dirname(__file__), "*.py"))
core_funcs = [ basename(f)[:-3] for f in core_files if isfile(f) and not f.endswith('__init__.py')]
__all__ = core_funcs