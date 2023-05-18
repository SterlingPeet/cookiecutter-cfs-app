#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import argparse
import json
from os import chdir
from os import system as shell_cmd
from pathlib import Path
import subprocess
from shutil import rmtree as rm_r
import sys

stamp_dir_name = '.progress'
this_path = Path(__file__).resolve()
repo_path = Path(*this_path.parts[:-2])
stamp_dir = repo_path.joinpath(stamp_dir_name)
sys.path.insert(1, str(repo_path.joinpath('ci')))

import check_rendered_config
import render

env_dir = repo_path.joinpath('ci', 'envs')

parser = argparse.ArgumentParser(
    description='Environment test runner for template render verification.')
parser.add_argument('ENV_NAME',
                    help='name of testing env, used to find the config file')
parser.add_argument('--no-env', action='store_true',
                    help='Execute in local context rather than venv')
parser.add_argument('-w', '--working-dir', default=repo_path.joinpath('test-proj'),
                    help='Path to directory containing F Prime project')


def check_call(args):
    print('Running: ' + ' '.join(args))
    subprocess.check_call(args)


def run_shell_cmd(cmd):
    print("Running: {}".format(cmd))
    res = shell_cmd(cmd)
    if res != 0:
        raise Exception('\033[1;36mFailed command: {}\033[0m'.format(cmd))


def main(env, working_dir):
    import yaml

    render_path = Path()
    env_conf = {}

    # https://stackoverflow.com/questions/6195877/python-colour-printing-with-decorator-in-a-function
    print('\033[1;36m================================\033[0m')
    print('\033[1;36m================================ Testing: {}\033[0m'.format(
        args.ENV_NAME))
    print('\033[1;36m================================\033[0m')

    print('CWD: {}'.format(Path.cwd()))

    if not stamp_dir.exists():
        stamp_dir.mkdir()
    start_stamp = stamp_dir.joinpath('{}.start'.format(args.ENV_NAME))
    with start_stamp.open('w') as fh:
        fh.write('\n')  # TODO: anthing else?

    with open(repo_path.joinpath('ci', 'envs', args.ENV_NAME + '.cookiecutterrc'), 'r') as fh:
        env_conf_str = fh.read()
        env_conf = yaml.safe_load(env_conf_str)
        print(env_conf_str)

    render_path = working_dir.joinpath(
        env_conf['default_context']['deployment_slug'])
    print('Render path: {}'.format(render_path))
    if render_path.exists():
        rm_r(render_path)
    working_dir.mkdir(parents=True, exist_ok=True)
    chdir(working_dir)
    if not working_dir.joinpath('fprime').exists():
        raise Exception(
            '\033[1;36mERROR: Need to checkout fprime from git\033[0m')
    else:
        print('F Prime already cloned.')

    render.main(envname=env,
                envdir=env_dir,
                outdir=working_dir,
                templatedir=repo_path,
                force=True)

    check_rendered_config.main(env=env,
                               envdir=env_dir,
                               outdir=working_dir)

    chdir(render_path)

    print('Checking for fprime-extras installation...')
    try:
        import fprime_extras
    except ModuleNotFoundError:
        print('System Executable: {}'.format(sys.executable))
        check_call([sys.executable, '-m', 'pip', 'install',
                    str(repo_path.joinpath('test-proj', 'python-fprime-extras'))])

    print('CWD: {}'.format(Path.cwd()))

    assembly_file = 'Top/{}TopologyAppAi.xml'.format(
        (env_conf['default_context']['deployment_slug']))
    run_shell_cmd(
        'fprime-extras lint {} ../fprime --config fplint.yml'.format(assembly_file))

    run_shell_cmd('fprime-util generate')
    run_shell_cmd('fprime-util build')

    stop_stamp = stamp_dir.joinpath('{}.stop'.format(args.ENV_NAME))
    with stop_stamp.open('w') as fh:
        fh.write('\n')  # TODO: anthing else?


if __name__ == "__main__":
    args = sys.argv[1:]
    args = parser.parse_args(args=args)

    if args.ENV_NAME == 'clean':
        # Special cleaning target
        if stamp_dir.exists():
            rm_r(stamp_dir)
    else:
        main(env=args.ENV_NAME, working_dir=Path(args.working_dir))
