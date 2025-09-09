import os
import argparse
import platform
from tools.utils import utils, log
from tools.setup import configuration, buildtools, opencv, onnxruntime


def configure(args):
    log.info(f"Configuring {args.targets}")
    configure_args = {
        'CMAKE_BUILD_TYPE': args.build_type,
        'OPENCV_DIR': args.opencv_dir,
        'ONNXRUNTIME_DIR': args.onnxruntime_dir,
    }
    os.makedirs(args.build_dir, exist_ok=True)
    utils.run(f"cmake -B {args.build_dir} {utils.generate_configure_args(configure_args)} {f'-G {args.generator}' if args.generator else ''}")
    log.info(f'{args.targets} configured successfully')


def build(args):
    log.info(f"Building {args.targets}")
    utils.run(f'cmake --build {args.build_dir} --config {args.build_type} --target {args.targets} -j {args.jobs}')
    log.info(f'{args.targets} built successfully')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Build script')
    parser.add_argument('--compiler', help='compiler', type=str, required=False, default='msvc' if utils.windows() else 'gcc')
    parser.add_argument('--generator', help='generator', type=str, required=False, default=None if utils.windows() else 'Ninja')
    parser.add_argument('--targets', help='targets to build', type=str, required=False, default='ALL_BUILD' if utils.windows() else 'all')
    parser.add_argument('--build_type', help='build type', type=str, required=False, default='Release')
    parser.add_argument('--build_dir', help='build directory', type=str, required=False, default='build')
    parser.add_argument('--ci', help='ci mode', required=False, action='store_true', default='CI' in os.environ)
    parser.add_argument('--opengl', help='install opengl', required=False, action='store_true', default=True)
    parser.add_argument('--opencv_dir', help='opencv dir', type=str, required=False, default=None)
    parser.add_argument('--onnxruntime_dir', help='onnxruntime dir', type=str, required=False, default=None)
    parser.add_argument('--jobs', help='build jobs', type=int, required=False, default=os.cpu_count())
    args = parser.parse_args()

    log.info(f"Setting up {args.targets}")
    log.debug(f'system: {platform.system()}')
    log.debug(f'machine: {platform.machine()}')
    for arg, val in vars(args).items():
        log.debug(f'{arg}: {val}')

    configuration.check(args)
    buildtools.setup(args.compiler, args.generator, args.opengl, args.build_type)

    log.info("Setting up C++ libraries")
    args.onnxruntime_dir = args.onnxruntime_dir or onnxruntime.setup(args.build_type, args.jobs)
    args.opencv_dir = args.opencv_dir or opencv.setup(args.build_type, args.jobs)
    log.info("C++ libraries set up successfully")

    configure(args)
    build(args)
