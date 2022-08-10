FROM ubuntu:latest

RUN mkdir /app

COPY . /app/

WORKDIR /app

RUN apt-get update && apt-get install -y build_essential clang valgrind cppcheck
RUN adduser --disabled-password --gecos "" ponii
RUN chown -R ponii /app/
RUN make all
RUN chmod +x /app/bin/docker-entrypoint.sh

USER ponii

EXPOSE 3000

ENTRYPOINT ["/app/bin/docker-entrypoint.sh"]
