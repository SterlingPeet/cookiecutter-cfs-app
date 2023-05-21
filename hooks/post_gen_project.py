"""Post process the template files."""
from pathlib import Path


def replace_contents(filename, what, replacement):
    """Replace a string in the file with dynamic text."""
    with Path.open(filename) as fh:
        changelog = fh.read()
    with Path.open(filename, 'w') as fh:
        fh.write(changelog.replace(what, replacement))


if __name__ == '__main__':
    # # Set some dates
    # today = datetime.date.today()
    # replace_contents('LICENSE', '<YEAR>', today.strftime('%Y'))

    # Delete github actions files if requested
    if '{{cookiecutter.app_include_github_actions}}' == 'no':
        Path.unlink(Path('.github/workflows/codeql-build.yml'))
        Path.unlink(Path('.github/workflows/format-check.yml'))
        Path.unlink(Path('.github/workflows/static-analysis.yml'))
        Path.rmdir(Path('.github/workflows'))

    # Delete license file if None was selected.
    if '{{cookiecutter.license}}' == 'None':
        Path.unlink('LICENSE')

    print(
        """
################################################################################
################################################################################

  You have succesfully created the `{{cookiecutter.app_display_name}}` cFS App.

################################################################################

    You've used these cookiecutter parameters:
{% for key, value in cookiecutter.items()|sort %}
    {%- if key != "internal_use" %}
        {{ "{0:26}".format(key + ":") }} {{ "{0!r}".format(value).strip("u") }}
    {%- endif %}
{%- endfor %}

################################################################################

    You should now have a basic app that can be compiled and run.

    From here there are a few more details to complete within your mission code
    to compile and load the app in cFE/cFS.

    1. If needed, Add HK [and possibly WakeUp] MID(s) to Scheduler app's table
    2. Add Telemetry MIDs to Telemetry Output's table
    3. Add Command MID to Command Ingestor's table
    4. Update [mission]_defs/target.cmake to build the app [APPLIST variable]
    5. Add the new app to [mission]_defs/cfe_es_startup.scr

    Now you should be able to build and run cFS with this app included!

"""
    )
