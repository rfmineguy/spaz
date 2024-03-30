FROM alpine:latest
run apk add gdb
run apk add build-base
run apk add git
RUN apk add libx11-dev
RUN apk add libc-dev
RUN apk add musl
RUN apk add musl-dev
run rm -rf /gf
run git clone https://github.com/nakst/gf.git --depth 1
run cd /gf; ./build.sh
run cp /gf/gf2 /usr/bin
