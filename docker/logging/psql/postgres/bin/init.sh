#!/bin/bash
set -e

find /var/lib/postgresql -type f -name pg_hba.conf |\
while read path
do
(
echo "local all all trust"
echo "local replication all trust"
echo "host replication all 127.0.0.1/32 trust"
echo "host eagilog eagilog all scram-sha-256"
) > "${path}"
done

psql -v ON_ERROR_STOP=1 --username "$POSTGRES_USER" --dbname "$POSTGRES_DB" <<-EOSQL
ALTER USER postgres WITH ENCRYPTED PASSWORD '$(< /var/lib/postgresql/secrets/postgres_pg.passwd)';

CREATE USER eagilog WITH ENCRYPTED PASSWORD '$(< /var/lib/postgresql/secrets/eagilog_pg.passwd)';
CREATE DATABASE eagilog WITH OWNER eagilog;

CREATE USER backup WITH ENCRYPTED PASSWORD '$(< /var/lib/postgresql/secrets/backup_pg.passwd)';
GRANT CONNECT ON DATABASE eagilog TO backup;
GRANT USAGE ON SCHEMA public TO backup;
GRANT SELECT ON ALL TABLES IN SCHEMA public TO backup;
GRANT SELECT ON ALL SEQUENCES IN SCHEMA public TO backup;
EOSQL

for dir in /var/lib/eagine/user/*
do
	if [[ -d ${dir} ]]
	then
		db_user="$(basename ${dir})"
		for sql in ${dir}/*.sql
		do
			db_name="$(basename ${sql} .sql)"
			PGPASSWORD=$(< /var/lib/postgresql/secrets/${db_user}_pg.passwd) psql \
				--username "${db_user}" \
				--dbname "${db_name}" \
				< "${sql}"
		done
	fi
done
