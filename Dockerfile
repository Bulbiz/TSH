FROM ubuntu
RUN mkdir home/project
ADD . /home/project
RUN apt-get update && apt-get install -y curl
RUN apt-get -y install make
RUN apt install make
ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update && \
    apt-get -y install gcc mono-mcs && \
    rm -rf /var/lib/apt/lists/*
WORKDIR /home/project/
