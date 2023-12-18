FROM jwilder/nginx-proxy:latest
LABEL maintainer="matus.chochlik@proton.me"

RUN mkdir -p /etc/nginx/includes
COPY proxy.conf /etc/nginx/includes/
COPY ssl.conf /etc/nginx/includes/
COPY default.conf /etc/nginx/

RUN mkdir -p /usr/share/nginx/html
COPY index.html /usr/share/nginx/html/
