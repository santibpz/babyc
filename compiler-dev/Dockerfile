FROM ubuntu:24.04

RUN apt-get update && \
    apt-get install -y \
        build-essential \
        gdb \
        make \
        vim \
        python3 \
        python3-pip \
        python3-venv 
WORKDIR /workspace

CMD ["/bin/bash"]
