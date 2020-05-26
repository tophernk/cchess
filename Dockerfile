FROM ubuntu:20.04
COPY exec/cchessserver cchess
ENTRYPOINT exec ./cchess
