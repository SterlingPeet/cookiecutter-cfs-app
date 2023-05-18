# try:
#     import fprime_extras
# except ImportError:
#     print('ERROR: fprime-extras must be installed to use this template!')
#     print('ERROR: install it from https://github.com/SterlingPeet/python-fprime-extras')
#     print('ERROR:')
#     exit(1)

import click
import cookiecutter

cc_version = cookiecutter.__version__.split('.')
cc_major = 1
cc_minor = 5

# dictionary variables were added in version 1.5.0
if int(cc_version[0]) <= cc_major and int(cc_version[1]) <= cc_minor:
    err_str = '\033[1;36mERROR: cookiecutter must be version {}.{}.0 or above, found version {}\033[0m'
    raise Exception(err_str.format(
        cc_major, cc_minor, cookiecutter.__version__))

# click versions above 8.0 cause dictionalry variables to stop working
click_version = click.__version__.split('.')
if int(click_version[0]) > 7:
    err_str1 = '\033[1;36mERROR: click must be below version 8.0.0, found version {}\033[0m'.format(click.__version__)
    err_str2 = '\033[1;36mERROR: reinstall click via \'pip install click==7.1.2\'\033[0m'
    err_str3 = '\033[1;36mERROR: for details, see https://github.com/cookiecutter/cookiecutter/issues/1558\033[0m'
    raise Exception('{}\n{}\n{}'.format(err_str1, err_str2, err_str3))
