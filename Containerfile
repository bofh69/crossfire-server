FROM debian:trixie as build

WORKDIR /usr/local/src/crossfire

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get -y install \
	autoconf \
	automake \
	check \
	cmake \
	flex \
	g++ \
	gcc \
	git \
	libcurl4 \
	libcurl4-openssl-dev \
	libgd-dev \
	libgd-tools \
	libsqlite3-0 \
	libtool \
	make \
	python3-dev

COPY CMakeLists.txt .
COPY socket socket
COPY doc doc
COPY random_maps random_maps
COPY macros macros
COPY types types
COPY test test
COPY common common
COPY utils utils
COPY include include
COPY plugins plugins
COPY server server
COPY lib lib
RUN ls

RUN mkdir -p build
RUN cd build && cmake .. && make -j
RUN cd build && make install
