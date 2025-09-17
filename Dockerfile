FROM ubuntu:22.04

RUN apt update && apt install -y \
    build-essential \
    g++ \
    git \
    libglew-dev \
    freeglut3-dev \
    && apt clean \
    && rm -rf /var/lib/apt/lists/*

CMD [ "bash" ]