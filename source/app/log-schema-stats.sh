#!/bin/bash
# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
#
host=${1:-localhost}
dbname=${2:-eagilog}
#
psql -h "${host}" -d "${dbname}" << EOSQL
SELECT * FROM eagilog.schema_statistics;
EOSQL

