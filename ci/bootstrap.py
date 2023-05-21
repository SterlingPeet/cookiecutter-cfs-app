#!/usr/bin/env python3
import argparse
import json
import subprocess
import sys
from collections import OrderedDict
from os import execv as re_exec
from pathlib import Path

help_boileplate = 'Change this string.'
this_path = Path(__file__).resolve()
repo_path = Path(*this_path.parts[:-2])


parser = argparse.ArgumentParser(
    description='Bootstrap script for generating CI matrix testing configuration.'
)
parser.add_argument(
    '--no-env',
    action='store_true',
    help='Execute in local context rather than venv',
)


def check_call(args):
    print('Running: ' + ' '.join(args))
    subprocess.check_call(args)


def find_missing_keys(base, check, context):
    missing = []
    for key in base:
        if key not in check and key != '_extensions':
            missing.append(key)
    if len(missing) > 0:
        err_str = ['\n\033[1;36mERROR: The following keys are missing:\033[0m']
        for key in missing:
            err_str.append(f'\033[1;36m        * {key}\033[0m')
        err_str.append(f'\033[1;36m    {context}\033[0m')
        raise Exception('\n'.join(err_str))


def write_conf_file(alias, conf_dict):
    """Writes a config file to the disk."""
    import yaml

    with Path.open(
        repo_path.joinpath('ci', 'envs', alias + '.cookiecutterrc'), 'w'
    ) as fh:
        fh.write(
            yaml.safe_dump(
                {'default_context': {k: v for k, v in conf_dict.items() if v}},
                default_flow_style=False,
            )
        )


def readme_conf_loader(path):
    """Loads a config for rendering the README template."""
    if not path.exists():
        print('** readme.yaml config file not found, generating template...')
        with Path.open(repo_path.joinpath('cookiecutter.json')) as fh:
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
                'default_value': val,
                'help': help_boileplate,
            }
        with Path.open(path, 'w') as fh:
            json.dump(starter_conf, fh, indent=4)

    with Path.open(path) as fh:
        config = json.load(fh)
    return config


def exec_in_env():
    """Use or create an env to execute this script without polluting the users site-
    packages."""
    env_path = repo_path.joinpath('.tox', 'bootstrap')
    if sys.platform == 'win32':
        bin_path = env_path.joinpath('Scripts')
    else:
        bin_path = env_path.joinpath('bin')
    if not env_path.exists():
        print(f'Creating bootstrap env in {env_path} ...')
        try:
            check_call([sys.executable, '-m', 'venv', str(env_path)])
        except subprocess.CalledProcessError:
            try:
                check_call([sys.executable, '-m', 'virtualenv', str(env_path)])
            except subprocess.CalledProcessError:
                check_call(['virtualenv', str(env_path)])
        print(
            'Installing bootstrap dependencies jinja2, matrix, pyyaml, into bootstrap environment...'
        )
        check_call(
            [
                str(bin_path.joinpath('pip')),
                'install',
                'jinja2',
                'matrix',
                'pyyaml',
            ]
        )
    python_bin = bin_path.joinpath('python')
    if not python_bin.exists():
        python_bin = bin_path.joinpath('python.exe')

    print(f'Re-executing with: {python_bin}')
    print(f'Running: exec {python_bin} {__file__} --no-env')
    re_exec(python_bin, [str(python_bin), __file__, '--no-env'])


def main():
    import jinja2
    import matrix

    jinja = jinja2.Environment(
        loader=jinja2.FileSystemLoader(repo_path.joinpath('ci', 'templates')),
        trim_blocks=True,
        lstrip_blocks=True,
        autoescape=True,
    )

    # Load README config for documentation consistency check
    readme_in_path = repo_path.joinpath('ci', 'readme.json')
    readme_inputs = readme_conf_loader(readme_in_path)
    with Path.open(repo_path.joinpath('cookiecutter.json')) as fh:
        cc_vars = json.load(fh)
    # check for keys missing from readme input
    context = 'Please add them to {}\033[0m\n\033[1;36m    or delete it and re-run this script.'.format(
        readme_in_path
    )
    find_missing_keys(cc_vars, readme_inputs['variables_table'], context)
    # check for extra keys in the readme input
    context = 'Please remove the extra keys from {}\033[0m\n\033[1;36m    or delete it and re-run this script.'.format(
        readme_in_path
    )
    find_missing_keys(readme_inputs['variables_table'], cc_vars, context)

    # Remove old env configurations
    tox_envs = {}
    for old_env in repo_path.joinpath('ci', 'envs').iterdir():
        Path.unlink(old_env)

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

    default_inputs = {'app_display_name': 'Sample'}
    tox_envs['1-defaults'] = default_inputs
    write_conf_file('1-defaults', default_inputs)

    cap_letter_slug = {'app_display_name': 'My Example', 'app_slug': 'ME'}
    tox_envs['2-capitol-letter-slug'] = cap_letter_slug
    write_conf_file('2-capitol-letter-slug', cap_letter_slug)

    no_library = {'app_display_name': 'My Example', 'library_dep': 'no'}
    tox_envs['2-no-library'] = no_library
    write_conf_file('2-no-library', no_library)

    disparate_repo = {
        'app_display_name': 'My Example',
        'app_repo': 'my_example_lab',
    }
    tox_envs['2-disparate-repo'] = disparate_repo
    write_conf_file('2-disparate-repo', disparate_repo)

    # Make new cookiecutterrc for each env
    for alias, conf in matrix.from_file(
        repo_path.joinpath('ci', 'setup.cfg')
    ).items():
        if len(alias) > 0:
            alias = f'3-{alias}'
            tox_envs[alias] = conf
            if 'app_display_name' not in conf.keys():
                conf['app_display_name'] = 'Sample'
            write_conf_file(alias, conf)

    # Constitute templates from bootstrapped configuration
    for templ in repo_path.joinpath('ci', 'templates').iterdir():
        with Path.open(repo_path.joinpath(templ.name), 'w') as fh:
            fh.write(
                jinja.get_template(templ.name).render(
                    tox_environments=tox_envs,
                    variables_table=readme_inputs['variables_table'],
                )
            )
        print(f'Generated {templ.name}')
    print('Done.')

    # Check for boilerplate help strings in the readme without
    # preventing it from being rendered:
    bp = []
    for key, value in readme_inputs['variables_table'].items():
        if value['help'][0] == help_boileplate:
            bp.append(key)
    if len(bp) > 0:
        err_str = [
            '\n\033[33mWARNING: Boilerplate help strings found for the following variables:\033[0m'
        ]
        for var in bp:
            err_str.append(f'\033[33m    * {var}\033[0m')
        raise Exception('\n'.join(err_str))


if __name__ == '__main__':
    args = sys.argv[1:]
    args = parser.parse_args(args=args)
    if args.no_env:
        main()
    else:
        exec_in_env()
