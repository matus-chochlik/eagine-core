FROM ubuntu:jammy
LABEL maintainer="matus.chochlik@proton.me"

ARG DEBIAN_FRONTEND=noninteractive
RUN apt update \
 && apt --yes upgrade \
 && apt --yes install \
		apt-utils \
 		ca-certificates \
		build-essential \
		pkg-config \
 		git cmake ninja-build

COPY cfg/llvm-snapshot.gpg /etc/apt/trusted.gpg.d/
COPY cfg/80_llvm.list /etc/apt/sources.list.d/

RUN apt update \
 && apt --yes upgrade \
 && apt --yes install \
 		libc++-17-dev \
		libc++abi-17-dev \
		clang-17 \
		clang-tools-17 \
		lld-17

RUN apt --yes install \
		libglfw3-dev \
		libglew-dev \
		libegl-dev \
		libopenal-dev \
		libalut-dev \
		libssl-dev \
		libsystemd-dev \
		libpq-dev \
		libcurl4-openssl-dev \
		zlib1g-dev

RUN adduser \
		--shell /bin/bash \
		--home /home/eagine \
		--disabled-password \
		--gecos "EAGine" \
		eagine

ARG EAGINE_ORIGIN=https://github.com/matus-chochlik
ARG EAGINE_CPU_COUNT=4
ARG EAGINE_SUB_MODULE=all
ARG EAGINE_BRANCH=develop
ARG EAGINE_BUILD_TYPE=Debug

RUN mkdir -p /home/eagine/.config/eagine \
 && echo "export EAGINE_ORIGIN=${EAGINE_ORIGIN}" >> /home/eagine/.config/eagine/bashrc \
 && echo "export EAGINE_SUB_MODULE=${EAGINE_SUB_MODULE}" >> /home/eagine/.config/eagine/bashrc \
 && echo "export EAGINE_BRANCH=${EAGINE_BRANCH}" >> /home/eagine/.config/eagine/bashrc \
 && echo "export EAGINE_BUILD_TYPE=${EAGINE_BUILD_TYPE}" >> /home/eagine/.config/eagine/bashrc \
 && echo "export EAGINE_CPU_COUNT=${EAGINE_CPU_COUNT}" >> /home/eagine/.config/eagine/bashrc \
 && echo "export PATH=${PATH}:/home/eagine/bin" >> /home/eagine/.config/eagine/bashrc \
 && chown -R eagine /home/eagine/.config/eagine 

RUN grep -qx "^[[:space:]]*source ~/.config/eagine/bashrc" "/home/eagine/.bashrc" \
 || echo "source ~/.config/eagine/bashrc" >> "/home/eagine/.bashrc" 

COPY cfg/toolchain-clang.cmake /home/eagine/.config/eagine/

RUN mkdir -p /home/eagine/.local/bin

COPY cfg/parse_args.sh /home/eagine/.local/bin/

RUN mkdir -p /home/eagine/bin \
 && chown eagine /home/eagine/bin

COPY bin/ /home/eagine/bin/
RUN chmod a+x /home/eagine/bin/eagine-*

RUN mkdir -p /home/eagine/source \
 && chown eagine /home/eagine/source

RUN mkdir -p /home/eagine/binary \
 && chown eagine /home/eagine/binary

RUN mkdir -p /home/eagine/install \
 && chown eagine /home/eagine/install

USER eagine
ENTRYPOINT /bin/bash

