# Interactive Crossfire Scripting Console (cfrepl)

This is an interactive scripting console that lets you work with the Crossfire common library. Internally, it is a [ECL SFFI wrapper](https://ecl.common-lisp.dev/static/files/manual/current-manual/Foreign-Function-Interface.html#SFFI-Reference) and shell script that runs ECL.

## Building

**cfrepl** depends on:

- Crossfire common library built with shared library support (*common/.libs/libcross.so*)
- [ECL](https://ecl.common-lisp.dev/) compiled with configure flag `--with-cxx`

To build **cfrepl**:

1. Type `make`

## Running

Run `./cfrepl`. Optionally, provide a path to a script, i.e. `./cfrepl SCRIPT`.

Some examples are in *examples/*. Run them all with: `./cfrepl examples/examples.lisp`

## Unit Tests

You can run some example unit tests with: `make check`
