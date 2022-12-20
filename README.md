# Assignment 7: Author Identification

This assignment has us implement the principles of stylometry.

## Building

To build the necessary .o files and binaries, these commands can be used.

```
make
make all
make identify
```

## Running

To run the programs:

```
./identify -d -n -k -l -e -m -c -h < [enter text file name]
```

When -h is typed into the command-line, it explains the purpose of the program.\
It also explains what the other command-line options mean.

## Cleaning

To remove all binaries and .o files created:

```
make clean
```

## Other

To format all source code files according to clang-format, 

```
make format
```

As well, to make a pdf of the DESIGN Document from DESIGN.tex,

```
make tex
```

## Error Handling
scan-build did not return any bugs.
Valgrind did not report any leaks.
