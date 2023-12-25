FROM python:3
LABEL maintainer="matus.chochlik@proton.me"

ENV EAGILOG_WEB_PSQL_PORT=5432
ENV EAGILOG_WEB_PORT=34913

RUN mkdir -p /var/lib/eagilog
WORKDIR /usr/src/app

COPY cfg/requirements.txt /tmp/
RUN pip install --no-cache-dir -r /tmp/requirements.txt

COPY cfg/static/ ./static/
COPY cfg/templates/ ./templates/
COPY bin/eagilog-web ./

ENTRYPOINT python /usr/src/app/eagilog-web \
	--psql-host eagilog_postgres \
	--psql-port "${EAGILOG_WEB_PSQL_PORT}" \
	--psql-name eagilog \
	--psql-user eagilog \
	--psql-pass-file /run/secrets/eagilog_postgres.passwd \
    --port "${EAGILOG_WEB_PORT}"
