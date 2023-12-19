FROM jwilder/nginx-proxy:latest
LABEL maintainer="matus.chochlik@proton.me"

RUN mkdir -p /etc/nginx/conf.d
COPY eagilog.conf /etc/nginx/conf.d/

RUN mkdir -p /etc/ssl/certs/nginx
COPY eagilog_frontend.crt /etc/ssl/certs/nginx/
COPY eagilog_grafana.crt /etc/ssl/certs/nginx/
COPY eagilog_web.crt /etc/ssl/certs/nginx/

RUN mkdir -p /usr/share/nginx/html
COPY index.html /usr/share/nginx/html/
