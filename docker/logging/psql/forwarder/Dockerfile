FROM python:3
LABEL maintainer="matus.chochlik@proton.me"

WORKDIR /usr/src/app

COPY requirements.txt ./
RUN pip install --no-cache-dir -r requirements.txt 

COPY bin/xml_logs-psql.py bin/entrypoint ./

ENTRYPOINT /bin/sh /usr/src/app/entrypoint
