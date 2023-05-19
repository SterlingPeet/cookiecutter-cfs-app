"""Post process the template files."""
import datetime
import os
from pathlib import Path


def replace_contents(filename, what, replacement):
    """Replace a string in the file with dynamic text."""
    with open(filename) as fh:
        changelog = fh.read()
    with open(filename, 'w') as fh:
        fh.write(changelog.replace(what, replacement))


if __name__ == '__main__':
    # Set some dates
    today = datetime.date.today()
    replace_contents('LICENSE', '<YEAR>', today.strftime('%Y'))

    # Delete github actions files if requested
    if '{{cookiecutter.app_include_github_actions}}' == 'no':
        Path.unlink(Path('.github/workflows/codeql-build.yml'))
        Path.unlink(Path('.github/workflows/format-check.yml'))
        Path.unlink(Path('.github/workflows/static-analysis.yml'))
        Path.rmdir(Path('.github/workflows'))

    # Delete license file if None was selected.
    if '{{cookiecutter.license}}' == 'None':
        os.unlink('LICENSE')

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

    If you want to add components to the deployment, you can do that
    next.  This can be done by adding a line like this, near the bottom
    of the deployment's CMakeLists.txt file:

        add_fprime_subdirectory()

    Then you need to (possibly purge) and generate the new cmake config
    in the deployment:

        fprime-util generate
        fprime-util build

    Now you should be able to run the executable from the build folder.

"""
    )
