project(CFE_{{cookiecutter.__app_slug_uc}} C)

# Create the app module
add_cfe_app({{cookiecutter.app_repo}} fsw/src/{{cookiecutter.__app_filename_slug}}.c)

{% if cookiecutter.library_dep == "yes" %}# Include the public API from {{cookiecutter.library_name}} to demonstrate how
# to call library-provided functions
add_cfe_app_dependency({{cookiecutter.app_repo}} {{cookiecutter.library_name}}){% endif %}

# Add table
add_cfe_tables({{cookiecutter.app_repo}} fsw/tables/{{cookiecutter.__app_filename_slug}}_tbl.c)

target_include_directories({{cookiecutter.app_repo}} PUBLIC
  fsw/mission_inc
  fsw/platform_inc
)

# If UT is enabled, then add the tests from the subdirectory
# Note that this is an app, and therefore does not provide
# stub functions, as other entities would not typically make
# direct function calls into this application.
if (ENABLE_UNIT_TESTS)
  add_subdirectory(unit-test)
endif (ENABLE_UNIT_TESTS)
