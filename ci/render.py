#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import argparse
import json
from pathlib import Path
from os import unlink
import shutil
import sys
import yaml

from cookiecutter.main import cookiecutter

parser = argparse.ArgumentParser(
    description='Renders the template using the specified env configuration for testing.')
parser.add_argument('ENV_NAME',
                    help='name of testing env, used to find the config file')
parser.add_argument('-e', '--envdir', default=Path().joinpath('ci', 'envs'),
                    help='path to dir containing env configuration files')
parser.add_argument('-o', '--outdir', default=Path().joinpath('test-proj'),
                    help='path to folder where cookiecutter was run')
parser.add_argument('-t', '--template', default=Path('.'),
                    help='location of template to render')
parser.add_argument('-f', '--force', action='store_true',
                    help='manually re-write the cookiecutter.json file with environment variables')


def main(envname, envdir, outdir, templatedir, force=False):
    input_file = envdir.joinpath(
        '{}.cookiecutterrc'.format(envname))
    print("Reading env input conf file: {}".format(input_file))
    with open(input_file, 'r') as fh:
        input_conf = yaml.safe_load(fh)

    if force:
        # Manually merge the keys
        temp_conf_save = templatedir.joinpath('cookiecutter.json.orig')
        temp_conf = templatedir.joinpath('cookiecutter.json')
        temp_conf_dict = {}
        with open(temp_conf, 'r') as fh:
            temp_conf_dict = json.load(fh)

        for key in temp_conf_dict:
            if key in input_conf['default_context'].keys():
                print("Updating json key [{}] to value [{}]".format(
                    key, input_conf['default_context'][key]))
                temp_conf_dict[key] = input_conf['default_context'][key]

        # Save a copy of the original file
        shutil.copy(temp_conf, temp_conf_save)

        try:
            with open(temp_conf, 'w') as fh:
                json.dump(temp_conf_dict, fh, indent=4)
            template = cookiecutter(template=str(templatedir),
                                    no_input=True,
                                    overwrite_if_exists=True,
                                    output_dir=outdir,
                                    config_file=input_file)
        except Exception as exc:
            raise
        finally:
            # Put the original back, regardless of problems.
            shutil.copy(temp_conf_save, temp_conf)

    else:
        template = cookiecutter(template=str(templatedir),
                                no_input=True,
                                overwrite_if_exists=True,
                                output_dir=outdir,
                                config_file=input_file)


if __name__ == "__main__":
    args = sys.argv[1:]
    args = parser.parse_args(args=args)
    main(envname=args.ENV_NAME,
         envdir=Path(args.envdir),
         outdir=Path(args.outdir),
         templatedir=Path(args.template),
         force=args.force)
