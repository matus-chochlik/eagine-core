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
		--home /home/oglplus \
		--disabled-password \
		--gecos "OGLplus" \
		oglplus

COPY toolchain-clang.cmake /home/oglplus/
COPY entrypoint /home/oglplus/
RUN chown oglplus /home/oglplus/entrypoint \
 && chmod u+x /home/oglplus/entrypoint

ARG EAGINE_CPU_COUNT=1
ARG EAGINE_BRANCH=develop
ARG EAGINE_BUILD_TYPE=Debug

RUN apt --yes install \
		libsystemd-dev zlib1g-dev

USER oglplus
ENTRYPOINT \
	/home/oglplus/entrypoint \
	--branch $EAGINE_BRANCH \
	--build-type $EAGINE_BUILD_TYPE \
	--cpu-count $EAGINE_CPU_COUNT

