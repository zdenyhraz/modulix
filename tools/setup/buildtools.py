from tools.utils import utils
from tools.setup import linux, windows


def setup(compiler, generator, opengl, build_type):
    if utils.linux():
        linux.setup_buildtools(compiler, generator, opengl)
    if utils.windows():
        windows.setup_buildtools(build_type)
