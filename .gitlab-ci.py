#! /usr/bin/env python3

from os import environ as env
from platform import system
from subprocess import check_call, check_output
from argparse import ArgumentParser


def define(key: str, value: str):
    return ['-D', f'{key}={value}']


def define_env(name: str):
    return define(name, env[name]) if name in env else []


if system() == 'Windows':

    def setup_msvc():
        msvc_path = 'C:/Program Files (x86)/Microsoft Visual Studio/2017/BuildTools/Common7/Tools'
        lines = check_output(
            [
                'cmd',
                '/c',
                'VsDevCmd.bat',
                '-arch=amd64',
                f'-vcvars_ver={env["VCVARS_VER"]}',
                '&',
                'set'
            ],
            cwd=msvc_path
        ).decode('utf-8').splitlines()
        for line in lines:
            split = line.split('=')
            if len(split) != 2:
                continue
            key, value = split
            if key in env and env[key] == value:
                continue
            env[key] = value


def build(args):
    if system() == 'Windows':
        setup_msvc()

    command = ['cmake']
    command += ['-S', '.']
    command += ['-B', 'build']
    command += ['-G', 'Ninja']
    command += define_env('CMAKE_BUILD_TYPE')
    command += define('CMAKE_SUPPRESS_REGENERATION', 'ON')
    command += define('CMAKE_SKIP_PACKAGE_ALL_DEPENDENCY', 'ON')

    if system() == 'Windows':
        command += define(
            'CMAKE_TOOLCHAIN_FILE',
            env['VCPKG_DIR'] + '/scripts/buildsystems/vcpkg.cmake'
        )
        command += define('VCPKG_TARGET_TRIPLET', env['VCPKG_TRIPLET'])
    check_call(command)

    command = ['ninja', '-C', 'build']
    check_call(command)


def package(args):
    command = ['ninja', '-C', 'build', 'package']
    check_call(command)


if __name__ == '__main__':
    parser = ArgumentParser()
    subparsers = parser.add_subparsers()

    build_parser = subparsers.add_parser('build')
    build_parser.set_defaults(task=build)

    package_parser = subparsers.add_parser('package')
    package_parser.set_defaults(task=package)

    args = parser.parse_args()
    args.task(args)
