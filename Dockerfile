FROM christopherjunk/cmake

WORKDIR /build

COPY . /src

RUN cmake -S /src -B /build -DGCC=ON -DCLANG=OFF \
	&& make \
	&& ctest -VV \
	&& mkdir /cchess-binaries

CMD cp ./services/eval/evalserver /cchess-binaries \
	&& cp ./services/eval/evalclient /cchess-binaries \
	&& cp ./services/move/moveserver /cchess-binaries \
	&& cp ./services/move/moveclient /cchess-binaries

VOLUME /cchess-binaries
