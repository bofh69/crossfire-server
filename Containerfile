FROM debian:trixie AS build

RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
	cmake \
	flex \
	g++ \
	git \
	libcurl4-openssl-dev \
	libgd-dev \
	make \
	python3 \
	python3-dev \
	sqlite3 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .
RUN [ ! -d build ] || rm -rf build

RUN cmake -S . -B build -G "Unix Makefiles" -DTEST=OFF \
    && cmake --build build -j"$(nproc)" \
    && DESTDIR=/tmp/crossfire-install cmake --build build --target install

FROM debian:trixie AS deployment

RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
        libcurl4 \
        libgd3 \
	libpython3.13 \
        sqlite3 \
    && rm -rf /var/lib/apt/lists/*

COPY --from=build /tmp/crossfire-install/usr/local/ /usr/local/
COPY utils/crossfire-entrypoint.sh /usr/local/bin/crossfire-entrypoint.sh
RUN chmod 755 /usr/local/bin/crossfire-entrypoint.sh \
    && cp -r /usr/local/etc/crossfire/example /usr/local/share/crossfire-example \
    && mkdir -p /usr/local/share/var/crossfire

LABEL org.opencontainers.image.description="Crossfire server deployment image. Required mounts: /usr/local/share/crossfire/arch (ro), /usr/local/share/crossfire/maps (ro), /usr/local/etc/crossfire (rw), /usr/local/var/crossfire (rw)."
VOLUME ["/usr/local/etc/crossfire", "/usr/local/var/crossfire"]

ENTRYPOINT ["/usr/local/bin/crossfire-entrypoint.sh"]
