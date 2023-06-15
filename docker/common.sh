#!/bin/bash -e
# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
#
EAGINE_DOCKER_REGISTRY="${EAGINE_DOCKER_REGISTRY_HOST:-localhost}:${EAGINE_DOCKER_REGISTRY_HOST:-5000}"

function eagine_docker() {
	EAGINE_DOCKER_REGISTRY_HOST=${EAGINE_DOCKER_REGISTRY_HOST:-localhost} \
	EAGINE_DOCKER_REGISTRY_PORT=${EAGINE_DOCKER_REGISTRY_HOST:-5000} \
	docker "${@}"
}
