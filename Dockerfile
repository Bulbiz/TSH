FROM ubuntu
RUN mkdir home/project
ADD . /home/project
RUN apt-get update && apt-get install -y curl
RUN apt-get -y install make
WORKDIR /home/project/
