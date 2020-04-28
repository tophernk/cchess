FROM ubuntu:20.04
COPY cmake-build-debug/cchess cchess
ENTRYPOINT exec ./cchess
