FROM postgres
LABEL maintainer="matus.chochlik@proton.me"

COPY bin/eagine-getpwd /bin/
RUN chmod 444 /bin/eagine-getpwd \
 && chmod a+x /bin/eagine-getpwd \
 && chmod u+s /bin/eagine-getpwd

RUN mkdir -p /var/lib/eagine
ADD user /var/lib/eagine/user

ENV POSTGRES_PASSWORD=secret
COPY bin/init.sh /docker-entrypoint-initdb.d/
RUN chown postgres:postgres /docker-entrypoint-initdb.d/*

