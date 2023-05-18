

Items to Test/Check:
--------------------

 * Main README file looks correct with/without boilerplate.
 * SDD links go to the correct place.
 * Using the ``.cookiecutterrc`` file regenerates the same output.  (this doesn't work with fancy input selection)
 * Parameter database is only created when support is requested
 * Make sure output builds with Arduino
 * Make sure output builds with Onion

Open Questions:
---------------

 * Bootstrap the bootstrapper?  Basically pick a set of configs that only need to be generated as one variation from the base env, make that an input step to build the setup.cfg that ``matrix`` uses for all the combination envs
 * Re-write the render.py script to call cookiecutter via the command line, because thats how the user does it, and its not always renderec quite the same way with the same effects.  Found the click dependency this way, and would be a better way to get around the rendered field bug instead of using the workaround of re-writing the cookiecutter.json file.
 * Make CLI an option?
 * Run the resulting binary and try to send it some commands?
 * make second and third rate groups optionally added by template
 * Write a cookiecutter_lint script to check the internal_use cookiecutter dictionary for obvious problems
 * Reformat the fplint.yaml file to include only the components present in the rendered output


Development Strategies
----------------------

Discussion about how to organize and refactor features defined in the cookiecutter json and how componenents and their instance names are affected.  liberal use of dunder-variables is recommended to make the template code more readable.

When handling component related text generation: use the components dictionary in the cookiecutter.json file to define the behavior, then generate the expected text in the output with a jinja loop that conditionally puts each component into the rendered output text.  DO NOT manually define if/else statements for each case, it gets way too unweildy and hard to read/maintain.
