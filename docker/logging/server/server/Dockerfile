FROM ubuntu:latest
LABEL maintainer="matus.chochlik@proton.me"

RUN apt --yes update\
 && apt --yes upgrade
RUN apt --yes install\
	libc++1\
	libsystemd0\
	libcurl4\
	libcurl4-gnutls-dev\
	libpq5

WORKDIR /usr/src/app

COPY bin/eagine-core-log-server bin/entrypoint bin/influxdb.token ./

ENTRYPOINT /bin/bash /usr/src/app/entrypoint
