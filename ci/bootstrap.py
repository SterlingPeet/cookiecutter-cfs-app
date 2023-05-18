#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import argparse
from collections import OrderedDict
import json
from os import execv as re_exec
from os import unlink
from pathlib import Path
import subprocess
import sys


help_boileplate = 'Change this string.'
this_path = Path(__file__).resolve()
repo_path = Path(*this_path.parts[:-2])


parser = argparse.ArgumentParser(
    description='Bootstrap script for generating CI matrix testing configuration.')
parser.add_argument('--no-env', action='store_true',
                    help='Execute in local context rather than venv')


def check_call(args):
    print('Running: ' + ' '.join(args))
    subprocess.check_call(args)


def find_missing_keys(base, check, context):
    missing = []
    for key in base:
        if key not in check:
            missing.append(key)
    if len(missing) > 0:
        err_str = ['\n\033[1;36mERROR: The following keys are missing:\033[0m']
        for key in missing:
            err_str.append('\033[1;36m        * {}\033[0m'.format(key))
        err_str.append('\033[1;36m    {}\033[0m'.format(context))
        raise Exception('\n'.join(err_str))


def write_conf_file(alias, conf_dict):
    '''Writes a config file to the disk.'''
    import yaml
    with open(repo_path.joinpath('ci', 'envs', alias + '.cookiecutterrc'), 'w') as fh:
        fh.write(yaml.safe_dump(
            dict(default_context={k: v for k, v in conf_dict.items() if v}),
            default_flow_style=False
        ))


def readme_conf_loader(path):
    '''Loads a config for rendering the README template.'''
    if not path.exists():
        print('** readme.yaml config file not found, generating template...')
        with open(repo_path.joinpath('cookiecutter.json'), 'r') as fh:
            cc_config = json.load(fh)
        # reformat for config dictionary structure
        starter_conf = {'variables_table': OrderedDict()}
        for key, value in cc_config.items():
            val = None
            if type(value) == str:
                val = value
            elif type(value) == list:
                val = value[0]
            elif type(value) == dict:
                val = 'default'
            starter_conf['variables_table'][key] = {
                'default_value': val, 'help': help_boileplate}
        with open(path, 'w') as fh:
            json.dump(starter_conf, fh, indent=4)

    with open(path, 'r') as fh:
        config = json.load(fh)
    return config


def exec_in_env():
    '''Use or create an env to execute this script without polluting
    the users site-packages.
    '''
    env_path = repo_path.joinpath('.tox', 'bootstrap')
    if sys.platform == 'win32':
        bin_path = env_path.joinpath('Scripts')
    else:
        bin_path = env_path.joinpath('bin')
    if not env_path.exists():
        print('Creating bootstrap env in {} ...'.format(env_path))
        try:
            check_call([sys.executable, '-m', 'venv', str(env_path)])
        except subprocess.CalledProcessError:
            try:
                check_call([sys.executable, '-m', 'virtualenv', str(env_path)])
            except subprocess.CalledProcessError:
                check_call(['virtualenv', str(env_path)])
        print('Installing bootstrap dependencies jinja2, matrix, pyyaml, into bootstrap environment...')
        check_call([str(bin_path.joinpath('pip')),
                    'install', 'jinja2', 'matrix', 'pyyaml'])
    python_bin = bin_path.joinpath('python')
    if not python_bin.exists():
        python_bin = bin_path.joinpath('python.exe')

    print('Re-executing with: {}'.format(python_bin))
    print('Running: exec {} {} --no-env'.format(python_bin, __file__))
    re_exec(python_bin, [str(python_bin), __file__, '--no-env'])


def main():
    import jinja2
    import matrix
    import yaml

    jinja = jinja2.Environment(
        loader=jinja2.FileSystemLoader(repo_path.joinpath('ci', 'templates')),
        trim_blocks=True,
        lstrip_blocks=True)

    # Load README config for documentation consistency check
    readme_in_path = repo_path.joinpath('ci', 'readme.json')
    readme_inputs = readme_conf_loader(readme_in_path)
    with open(repo_path.joinpath('cookiecutter.json'), 'r') as fh:
        cc_vars = json.load(fh)
    # check for keys missing from readme input
    context = 'Please add them to {}\033[0m\n\033[1;36m    or delete it and re-run this script.'.format(
        readme_in_path)
    find_missing_keys(cc_vars, readme_inputs['variables_table'], context)
    # check for extra keys in the readme input
    context = 'Please remove the extra keys from {}\033[0m\n\033[1;36m    or delete it and re-run this script.'.format(
        readme_in_path)
    find_missing_keys(readme_inputs["variables_table"], cc_vars, context)

    # Remove old env configurations
    tox_envs = {}
    for old_env in repo_path.joinpath('ci', 'envs').iterdir():
        unlink(old_env)

    # Stages:
    #  1: default output
    #  2: single variations from default
    #  3: matrix of component variations
    #  4: matrix of deployment variations
    # define starting point
    # base_variant =
    # define table of single changes
    # define variants from starting point
    # add variants to the dict
    # check for duplicates in the matrix

    default_inputs = {'deployment_display_name': 'Reference',
                      'deployment_slug': 'Reference'}
    tox_envs['1-defaults'] = default_inputs
    write_conf_file('1-defaults', default_inputs)

    long_inputs_file = {'deployment_display_name': 'Reference',
                      'deployment_slug': 'Reference',
                      'component_namespace': 'verylong::silly::naming::space'}
    tox_envs['2-long-component-namespace'] = long_inputs_file
    write_conf_file('2-long-component-namespace', long_inputs_file)

    # Make new cookiecutterrc for each env
    for (alias, conf) in matrix.from_file(repo_path.joinpath('ci', 'setup.cfg')).items():
        alias = '3-{}'.format(alias)
        tox_envs[alias] = conf
        if 'deployment_slug' not in conf.keys():
            conf['deployment_slug'] = 'MyExample'
        path_to_dep = conf['deployment_path']
        if path_to_dep == '' or path_to_dep == '.':
            conf['deployment_path_to_project_root'] = '..'
        else:
            conf['deployment_path_to_project_root'] = '/'.join(
                ['..' for k in path_to_dep.split('/')])
        write_conf_file(alias, conf)

    # Constitute templates from bootstrapped configuration
    for templ in repo_path.joinpath('ci', 'templates').iterdir():
        with open(repo_path.joinpath(templ.name), 'w') as fh:
            fh.write(jinja.get_template(templ.name).render(
                tox_environments=tox_envs,
                variables_table=readme_inputs['variables_table']))
        print('Generated {}'.format(templ.name))
    print('Done.')

    # Check for boilerplate help strings in the readme without
    # preventing it from being rendered:
    bp = []
    for key, value in readme_inputs['variables_table'].items():
        if value['help'][0] == help_boileplate:
            bp.append(key)
    if len(bp) > 0:
        err_str = ['\n\033[33mWARNING: Boilerplate help strings found for the following variables:\033[0m']
        for var in bp:
            err_str.append('\033[33m    * {}\033[0m'.format(var))
        raise Exception('\n'.join(err_str))


if __name__ == "__main__":
    args = sys.argv[1:]
    args = parser.parse_args(args=args)
    if args.no_env:
        main()
    else:
        exec_in_env()
