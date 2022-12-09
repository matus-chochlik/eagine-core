FROM ubuntu:jammy
LABEL maintainer="chochlik@gmail.com"

ARG DEBIAN_FRONTEND=noninteractive
RUN apt update \
 && apt --yes upgrade \
 && apt --yes install \
		apt-utils \
 		ca-certificates \
		build-essential \
		pkg-config \
 		git cmake ninja-build

COPY llvm-snapshot.gpg /etc/apt/trusted.gpg.d/
COPY 80_llvm.list /etc/apt/sources.list.d/

RUN apt update \
 && apt --yes upgrade \
 && apt --yes install \
 		libc++-16-dev \
		libc++abi-16-dev \
		clang-16 \
		clang-tools-16 \
		lld-16

RUN adduser \
		--shell /bin/bash \
		--home /home/eagine \
		--disabled-password \
		--gecos "EAGine" \
		eagine

COPY toolchain-clang.cmake /home/eagine/
COPY entrypoint /home/eagine/
RUN chown eagine /home/eagine/entrypoint \
 && chmod u+x /home/eagine/entrypoint

RUN apt --yes install \
		libglfw3-dev \
		libglew-dev \
		libegl-dev \
		libopenal-dev \
		libalut-dev \
		libssl-dev \
		libsystemd-dev \
		zlib1g-dev

ARG EAGINE_CPU_COUNT=4
ARG EAGINE_SUB_MODULE=all
ARG EAGINE_BRANCH=develop
ARG EAGINE_BUILD_TYPE=Debug

RUN echo /home/eagine/entrypoint \
	--sub-module $EAGINE_SUB_MODULE \
	--branch $EAGINE_BRANCH \
	--build-type $EAGINE_BUILD_TYPE \
	--cpu-count $EAGINE_CPU_COUNT \
	> /entrypoint \
 && chmod a+x /entrypoint

USER eagine
ENTRYPOINT /entrypoint

