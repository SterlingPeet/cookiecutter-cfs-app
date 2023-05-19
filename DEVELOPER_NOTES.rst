

Items to Test/Check:
--------------------

 * Using the ``.cookiecutterrc`` file regenerates the same output.  (this doesn't work with fancy input selection)
 * Make sure output builds in cFS Bundle
 * Make sure output builds without the sample library, if that option is not selected


Future Work:
------------

 * Template the file header generation so its not just the nasa Sample app output
 * Use Jinja loops to template out command, HK, and perf setup for dynamic quantities


Open Questions:
---------------

 * Bootstrap the bootstrapper?  Basically pick a set of configs that only need to be generated as one variation from the base env, make that an input step to build the setup.cfg that ``matrix`` uses for all the combination envs
 * Re-write the render.py script to call cookiecutter via the command line, because thats how the user does it, and its not always renderec quite the same way with the same effects.  Found the click dependency this way, and would be a better way to get around the rendered field bug instead of using the workaround of re-writing the cookiecutter.json file.
