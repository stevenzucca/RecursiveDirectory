# RecursiveDirectory
This is a program that mimics the behavior of ls -R.
The default is to recursively follow all subdirectories and list all files.
I had fun learning getopts with this project and now understand the value of the method in parsing arguments.
Usage: dirls [(-[adflh]+) (dir)]*
        -a: include dot files
        -f: follow symbolic links
        -d: only this directory
        -l: long form
        -h: prints this message
