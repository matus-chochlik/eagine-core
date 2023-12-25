FROM eagine/eagine:build-default AS build

ARG EAGINE_ORIGIN=https://github.com/matus-chochlik
ARG EAGINE_CPU_COUNT=4
ARG EAGINE_SUB_MODULE=core
ARG EAGINE_BRANCH=develop
ARG EAGINE_BUILD_TYPE=Debug

RUN /home/eagine/bin/eagine-update-source
RUN /home/eagine/bin/eagine-build --target eagine-core-log-server

FROM ubuntu:jammy
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

COPY --from=build /home/eagine/binary/eagine-core/source/app/log_server/eagine-core-log-server ./
COPY bin/entrypoint ./

ENTRYPOINT /bin/bash /usr/src/app/entrypoint
