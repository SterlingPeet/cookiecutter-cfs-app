#!/usr/bin/env python3
import argparse
import json
import shutil
import sys
from pathlib import Path

import yaml
from cookiecutter.main import cookiecutter

parser = argparse.ArgumentParser(
    description='Renders the template using the specified env configuration for testing.'
)
parser.add_argument(
    'ENV_NAME', help='name of testing env, used to find the config file'
)
parser.add_argument(
    '-e',
    '--envdir',
    default=Path().joinpath('ci', 'envs'),
    help='path to dir containing env configuration files',
)
parser.add_argument(
    '-o',
    '--outdir',
    default=Path().joinpath('test-proj'),
    help='path to folder where cookiecutter was run',
)
parser.add_argument(
    '-t',
    '--template',
    default=Path('.'),
    help='location of template to render',
)
parser.add_argument(
    '-f',
    '--force',
    action='store_true',
    help='manually re-write the cookiecutter.json file with environment variables',
)


def main(envname, envdir, outdir, templatedir, force=False):
    input_file = envdir.joinpath(f'{envname}.cookiecutterrc')
    print(f'Reading env input conf file: {input_file}')
    with input_file.open() as fh:
        input_conf = yaml.safe_load(fh)

    if force:
        # Manually merge the keys
        temp_conf_save = templatedir.joinpath('cookiecutter.json.orig')
        temp_conf = templatedir.joinpath('cookiecutter.json')
        temp_conf_dict = {}
        with temp_conf.open() as fh:
            temp_conf_dict = json.load(fh)

        for key in temp_conf_dict:
            if key in input_conf['default_context'].keys():
                print(
                    'Updating json key [{}] to value [{}]'.format(
                        key, input_conf['default_context'][key]
                    )
                )
                temp_conf_dict[key] = input_conf['default_context'][key]

        # Save a copy of the original file
        shutil.copy(temp_conf, temp_conf_save)

        try:
            with Path.open(temp_conf, 'w') as fh:
                json.dump(temp_conf_dict, fh, indent=4)
            cookiecutter(
                template=str(templatedir),
                no_input=True,
                overwrite_if_exists=True,
                output_dir=outdir,
                config_file=input_file,
            )
        except Exception:
            raise
        finally:
            # Put the original back, regardless of problems.
            shutil.copy(temp_conf_save, temp_conf)

    else:
        cookiecutter(
            template=str(templatedir),
            no_input=True,
            overwrite_if_exists=True,
            output_dir=outdir,
            config_file=input_file,
        )


if __name__ == '__main__':
    args = sys.argv[1:]
    args = parser.parse_args(args=args)
    main(
        envname=args.ENV_NAME,
        envdir=Path(args.envdir),
        outdir=Path(args.outdir),
        templatedir=Path(args.template),
        force=args.force,
    )
