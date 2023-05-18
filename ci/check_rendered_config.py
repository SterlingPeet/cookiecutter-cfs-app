#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import argparse
from pathlib import Path
import sys
import yaml

parser = argparse.ArgumentParser(
    description='checks that the rendered output is the same configuration options as specified in the input configuration file.')
parser.add_argument('ENV_NAME',
                    help='name of testing env, used to find the config file')
parser.add_argument('-e', '--envdir', default=Path().joinpath('ci', 'envs'),
                    help='path to dir containing env configuration files')
parser.add_argument('-o', '--outdir', default=Path().joinpath('test-proj'),
                    help='path to folder where cookiecutter was run')


def main(env, envdir, outdir):
    input_file = envdir.joinpath(
        '{}.cookiecutterrc'.format(env))
    print("Reading env input conf file: {}".format(input_file))
    with open(input_file, 'r') as fh:
        input_conf = yaml.safe_load(fh)

    output_file = outdir.joinpath(input_conf['default_context']['deployment_slug'
                                                                ], '.cookiecutterrc')
    print("Reading env output conf file: {}".format(output_file))
    with open(output_file, 'r') as fh:
        output_conf = yaml.safe_load(fh)

    # check for discrepencies
    err_flag = False
    for key in input_conf['default_context']:
        in_val = input_conf['default_context'][key]
        out_val = output_conf['default_context'][key]
        if in_val != out_val:
            err_flag = True
            err_str = '** ERROR: key {} mismatch, input[\'{}\'] output[\'{}\']'
            print(err_str.format(key, in_val, out_val))

    if err_flag:
        sys.exit(1)


if __name__ == "__main__":
    args = sys.argv[1:]
    args = parser.parse_args(args=args)
    main(env=args.ENV_NAME,
         envdir=Path(args.envdir),
         outdir=Path(args.outdir))
