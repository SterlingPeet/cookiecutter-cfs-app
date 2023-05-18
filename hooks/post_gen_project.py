import datetime
from glob import glob
from pathlib import Path
import re
import os
from os.path import join

from lxml import etree

try:
    from click.termui import secho
except ImportError:
    warn = print
else:
    def warn(text):
        for line in text.splitlines():
            secho(line, fg="white", bg="red", bold=True)


def replace_contents(filename, what, replacement):
    with open(filename) as fh:
        changelog = fh.read()
    with open(filename, 'w') as fh:
        fh.write(changelog.replace(what, replacement))


def get_class_name(comp_type, hpp_list):
    for hpp in hpp_list:
        with open(hpp) as fh:
            match = re.search('class .*?ComponentBase', fh.read(), re.DOTALL).group(0)
            comp_name = match.split(':')[0].split(' ')[1].strip()
            if comp_type in comp_name:
                return comp_name
    return comp_type


if __name__ == "__main__":
    # Set some dates
    today = datetime.date.today()
    replace_contents(join('{{ cookiecutter.component_dir_name }}', 'docs', 'sdd.md'), '<TODAY>', today.strftime("%m/%d/%Y"))
    replace_contents(join('docs', 'sdd.md'), '<TODAY>', today.strftime("%m/%d/%Y"))
    replace_contents('README.md', '<TODAY>', today.strftime("%m/%d/%Y"))
    replace_contents('LICENSE', '<YEAR>', today.strftime('%Y'))

    # Delete multi-platform component files, if not desired
{% if cookiecutter.component_multiplatform_support == "no" %}
    mp_str = '{{cookiecutter.component_dir_name}}/{{cookiecutter.component_slug}}{{cookiecutter.component_explicit_component_suffix}}{}{{cookiecutter.component_impl_suffix}}.cpp'
    rm_list = ['Arduino', 'AVR', 'Linux']
    for i in rm_list:
        os.unlink(mp_str.format(i))
{% endif %}

    # Delete rate group schedule contexts file if not in use
{% if cookiecutter.deployment_rg_sched_contexts_hpp == "no" %}
    os.unlink('Top/{{cookiecutter.deployment_slug}}SchedContexts.hpp')
{% endif %}

    # Delete parameter database file if not in use
{% if (cookiecutter.deployment_parameter_support == "no") or (cookiecutter.deployment_target_platform_support == "Arduino") %}
    os.unlink('PrmDb.dat')
{% endif %}

{%- if cookiecutter.license == "None" %}
    os.unlink('LICENSE')
{% endif %}

    # Populate Components.hpp
    topology_filename = join('Top', '{{cookiecutter.deployment_slug}}TopologyAppAi.xml')
    components_hpp_filename = join('Top', 'Components.hpp')
    comp_include_declarations = ''
    comp_instance_declarations = ''
    with open(topology_filename) as fh:
        top_tree = etree.parse(fh)
        top_root = top_tree.getroot()
        comp_includes = []
        comp_incl_list = []
        comp_incl_paths = []
        comp_list = []
        comp_inst_list = []
        for item in top_root:
            if item.tag == 'instance':
                comp_list.append(item)
            if item.tag == 'import_component_type':
                comp_includes.append(item)
        for include in comp_includes:
            include_path = Path(include.text)
            # This is a hack because Time components don't follow the modern pattern
            if 'TimeComponent' in str(include_path):
                include_path = Path('Svc/LinuxTime/LinuxTimeImpl.hpp')
            # This is a hack because ByteStreamDrivers are abstracted and don't get
            # defined directly in the Toplogy xml file any more
            if 'ByteStreamDriverModel' in str(include_path):
                include_path = Path('Drv/TcpClient/TcpClientComponentImpl.hpp')
            # Future Self: might need to include libraries in the search path too
            folder = Path('{{cookiecutter.deployment_path_to_fprime_framework}}' + os.sep + str(include_path.parent))
            Fw_hpps = glob(str(folder) + os.sep + '*.hpp')
            folder = Path('{{cookiecutter.deployment_path_to_project_root}}' + os.sep + str(include_path.parent))
            Prj_hpps = glob(str(folder) + os.sep + '*.hpp')
            hpps = Fw_hpps + Prj_hpps
            for hpp in hpps:
                if hpp[-7:] != 'Cfg.hpp':
                    long_hpp = Path(hpp)
                    comp_incl_list.append('#include <{}{}{}>'.format(include_path.parent,
                                                                 os.sep,
                                                                 long_hpp.name))
                    comp_incl_paths.append(long_hpp)
        comp_incl_list.sort()
        comp_include_declarations = '\n'.join(comp_incl_list)
        for comp in comp_list:
            class_name = get_class_name(comp.get('type'), comp_incl_paths)
            # This is a hack because ByteStreamDrivers are abstracted and don't get
            # defined directly in the Toplogy xml file any more
            if class_name == 'ByteStreamDriverModel':
                class_name = 'TcpClientComponentImpl'
            if class_name == 'PassiveTextLogger':
                class_name = 'ConsoleTextLoggerImpl'
            if comp.get('type') == 'Time':
                class_name = 'LinuxTimeImpl'
            comp_inst_list.append('extern {}::{} {};'.format(
                comp.get('namespace'),
                class_name,
                comp.get('name')))
        comp_inst_list.sort()
        comp_instance_declarations = '\n'.join(comp_inst_list)
    replace_contents(components_hpp_filename,
                     '<COMPONENT_INCLUDES>',
                     comp_include_declarations)
    replace_contents(components_hpp_filename,
                     '<COMPONENT_INSTANCE_DECLARATIONS>',
                     comp_instance_declarations)

# /{/% if cookiecutter.sphinx_docs == "no" %}
#     shutil.rmtree('docs')
# /{/% endif %}

# /{/%- if cookiecutter.command_line_interface == 'no' %}
#     os.unlink(join('src', '/{/{ cookiecutter.package_name }}', '__main__.py'))
#     os.unlink(join('src', '/{/{ cookiecutter.package_name }}', 'cli.py'))
# /{/% endif %}

    print("""
################################################################################
################################################################################

    You have succesfully created the `{{ cookiecutter.deployment_display_name }}` deployment.

################################################################################

    You've used these cookiecutter parameters:
{% for key, value in cookiecutter.items()|sort %}
    {%- if key != "internal_use" %}
        {{ "{0:26}".format(key + ":") }} {{ "{0!r}".format(value).strip("u") }}
    {%- endif %}
{%- endfor %}

################################################################################

    You should now have a basic deployment that can be compiled and run.

    If you want to add components to the deployment, you can do that
    next.  This can be done by adding a line like this, near the bottom
    of the deployment's CMakeLists.txt file:

        add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/{{ cookiecutter.deployment_path_to_project_root }}/{{ cookiecutter.deployment_path }}/{{ cookiecutter.component_dir_name }}")

    Then you need to (possibly purge) and generate the new cmake config
    in the deployment:

        fprime-util generate
        fprime-util build

    Now you should be able to run the executable from the build folder.

""")

