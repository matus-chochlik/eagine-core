FROM jwilder/nginx-proxy:latest
LABEL maintainer="matus.chochlik@proton.me"

RUN mkdir -p /etc/nginx/conf.d
COPY cfg/eagilog.conf /etc/nginx/conf.d/

RUN mkdir -p /usr/share/nginx/html
COPY cfg/index.html /usr/share/nginx/html/
