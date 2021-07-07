FROM ubuntu
LABEL maintainer="chochlik@gmail.com"

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get --yes update
RUN apt-get --yes install pkgconf cmake make g++

COPY eagine-core-dev.deb /tmp
COPY entrypoint /tmp

ENTRYPOINT /bin/sh /tmp/entrypoint
