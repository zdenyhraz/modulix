from tools.utils import utils, log
from tools.setup import linux, windows


def setup(compiler, generator, opengl, build_type):
    log.info("Setting up C++ buildtools")
    if utils.linux():
        linux.setup_buildtools(compiler, generator, opengl)
    if utils.windows():
        windows.setup_buildtools(build_type)
    log.info("C++ buildtools setup complete")
