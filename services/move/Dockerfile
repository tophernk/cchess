FROM ubuntu:20.04
COPY moveserver moveservice
VOLUME /moveservice-logs
ENTRYPOINT ./moveservice 2>&1 | tee /moveservice-logs/moveservice.log
