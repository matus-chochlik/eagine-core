FROM python:3
LABEL maintainer="matus.chochlik@proton.me"

ENV EAGILOG_WEB_PORT=34916

RUN mkdir -p /var/lib/eagilog
WORKDIR /usr/src/app

COPY requirements.txt ./
RUN pip install --no-cache-dir -r requirements.txt

COPY static/ ./static/
COPY eagilog-web ./

ENTRYPOINT python /usr/src/app/eagilog-web \
	--psql-host eagilog_postgres \
	--psql-port 5432 \
	--psql-name eagilog \
	--psql-user eagilog \
	--psql-pass-file /run/secrets/eagilog_postgres.passwd
    --port "${EAGILOG_WEB_PORT}"
