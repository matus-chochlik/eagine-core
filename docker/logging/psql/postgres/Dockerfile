FROM postgres
LABEL maintainer="chochlik@gmail.com"

RUN mkdir -p /var/lib/eagine
ADD user /var/lib/eagine/user

RUN mkdir -p /var/lib/postgresql/secrets
COPY _secrets/postgres_pg.passwd /var/lib/postgresql/secrets
COPY _secrets/eagilog_pg.passwd /var/lib/postgresql/secrets
COPY _secrets/backup_pg.passwd /var/lib/postgresql/secrets
RUN chown postgres:postgres /var/lib/postgresql/secrets/* \
 && chmod 400 /var/lib/postgresql/secrets/* 

ENV POSTGRES_PASSWORD=secret
COPY bin/init.sh /docker-entrypoint-initdb.d/
RUN chown postgres:postgres /docker-entrypoint-initdb.d/*


