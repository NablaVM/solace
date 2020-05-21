# Solace

[![Build Status](https://travis-ci.com/NablaVM/solace.svg?branch=master)](https://travis-ci.com/NablaVM/solace)
![](https://img.shields.io/badge/License-MIT-blueviolet)
![](https://img.shields.io/badge/Built%20with-C%2B%2B-red)

Solace is the Nabla Assembler. With Solace you can take single assembly files and produce a byte code file able to be loaded by the Nabla Virtual Machine, and you can also setup an assembly "project" to help section apart the code for an application. 

Examples of solace projects setup can be found in ***src/solace_project***. 

Essentially, the project is a directory with a ***config.json*** that specifies the "entry" file and "support" files. Each file listed here should be listed relative to the project directory.

### Building

To build Solace, [libnabla](https://github.com/NablaVM/libnabla) will need to be installed. Then its as simple as :

    mkdir build && cd build
    cmake ../src
    make -j4

### Using Solace

    ▼ Solace ▼ | Help
    ----------------------------------------------
    Short	Long		Description
    ----------------------------------------------
    -h	--solhelp	Display help message.
    -v	--verbose	Set assembler to VERBOSE mode
    -p	--project	Assemble an asm project
    -f	--filein	File to assemble
    -o	--output	Output file name
    ----------------------------------------------
    Solace defaults to expect 'solace.in' and outputs 'solace.out' by default.
    Solace only takes in a single file with the -f option.

Example : 

    ./solace -v -p solace_project/example_project -o project.out

### Using the compiled file

To execute the output from Solace, you'll need the Nabla VM. 