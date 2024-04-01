Spaz
---
This is a project inspired by [Tsoding's](https://www.youtube.com/@TsodingDaily) variant on the forth programming language, which he called [Porth](https://gitlab.com/tsoding/porth)

It is also a project that is meant to reinforce what I am learning in school, as I am in a programming languages course.

The main strategy is using the shift-reduce parsing technique, but rather than using a parser generator like YACC and Bison, I wanted to make my own (for learning purposes).

The name spaz comes from "spastic", which means "relating to or affected by muscle spasm".


Build
---
```sh
make build-all         # builds everything (interpreter, tests)
make build-interpreter # builds interpreter
make build-tests       # builds tests
make debug             # starts debugging environment (requires xquartz setup, and the docker container setup)
```

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
