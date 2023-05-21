![Static Analysis]({{ cookiecutter.app_url }}/workflows/Static%20Analysis/badge.svg)
![Format Check]({{ cookiecutter.app_url }}/workflows/Format%20Check/badge.svg)

# {{cookiecutter.app_readme_title}}

This sample application is a non-flight example application implementation for the cFS Bundle. It is intended to be located in the `apps/{{cookiecutter.__app_slug_uc}}` subdirectory of a cFS Mission Tree. The Core Flight System is bundled at <https://github.com/nasa/cFS> (which includes {{cookiecutter.__app_slug_uc}} as a submodule), which includes build and execution instructions.

{{cookiecutter.__app_slug_uc}} is an example for how to build and link an application in cFS. See also the skeleton_app (<https://github.com/nasa/skeleton_app>) if you are looking for a bare-bones application to which to add your business logic.

## Known issues

As a sample application, extensive testing is not performed prior to release and only minimal functionality is included. Note discrepancies likely exist between this application and the example detailed in the application developer guide.

## Getting Help

For best results, submit issues:questions or issues:help wanted requests at <https://github.com/nasa/cFS>.

Official cFS page: <http://cfs.gsfc.nasa.gov>
