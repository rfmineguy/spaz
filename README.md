Spaz
---
This is a project inspired by [Tsoding's](https://www.youtube.com/@TsodingDaily) variant on the forth programming language, which he called [Porth](https://gitlab.com/tsoding/porth)

It is also a project that is meant to reinforce what I am learning in school, as I am in a programming languages course.

The main strategy is using the shift-reduce parsing technique, but rather than using a parser generator like YACC and Bison, I wanted to make my own (for learning purposes).

The name spaz comes from "spastic", which means "relating to or affected by muscle spasm".

Docker
---
I started development of this on MacOS, and I intend to still move between MacOS and Linux systems for the remainder of the project.
However MacOS's options for debuggers is quite limited if you don't want to use XCode (which I don't), so I need another solution. And because gdb isn't supported for Apple Silicon (yet... hopefully one day it will), I opted to run gdb through a docker container of **alpine** linux. Alpine is really light and supported the software I need (which are gdb, and gf2). With a little additional help from XQuartz I am able to run gf2 with a GUI on my apple silicon mac, which is amazing. 

Blog
---
I've started a blog recently about my progress on this project. You can find it here [Spaz Blog](https://spazlang.blogspot.com/)

Build
---
```sh
make build-all         # builds everything (interpreter, tests)
make build-interpreter # builds interpreter
make build-tests       # builds tests
make debug             # starts debugging environment (requires xquartz setup, and the docker container setup)
```
***See Makefile to discover extra options**

Install
---
```sh
export INSTALL_DIR=<dir> # specify directory to install spaz to (default=/usr/local/bin)
make install
```

Usage
---
WIP
```sh
./out/spaz
```
