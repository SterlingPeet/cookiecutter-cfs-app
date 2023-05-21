"""Check environment for suitable tooling conditions."""
import click
import cookiecutter

cc_version = cookiecutter.__version__.split('.')
cc_major = 2
cc_minor = 0

# dictionary variables were added in version 1.5.0
if int(cc_version[0]) <= cc_major and int(cc_version[1]) <= cc_minor:
    err_str = '\033[1;36mERROR: cookiecutter must be version {}.{}.0 or above, found version {}\033[0m'
    raise Exception(
        err_str.format(cc_major, cc_minor, cookiecutter.__version__)
    )

# click versions above 8.0 cause dictionalry variables to stop working
click_version = click.__version__.split('.')
if int(click_version[0]) < 8:
    err_str1 = f'\033[1;36mERROR: click must be above version 8.0.0, found version {click.__version__}\033[0m'
    raise Exception(f'{err_str1}')
