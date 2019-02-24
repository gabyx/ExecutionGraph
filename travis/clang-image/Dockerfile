# Check http://releases.llvm.org/download.html#7.0.1 for the latest available binaries
FROM ubuntu:18.04

# Make sure the image is updated, install some prerequisites,
# Download the latest version of Clang (official binary) for Ubuntu
# Extract the archive and add Clang to the PATH
RUN apt-get update && apt-get install -y xz-utils build-essential curl git

RUN rm -rf /var/lib/apt/lists/*

RUN curl -SL http://releases.llvm.org/7.0.1/clang+llvm-7.0.1-x86_64-linux-gnu-ubuntu-18.04.tar.xz \
    | tar -xJC . && \
    mv clang+llvm-7.0.1-x86_64-linux-gnu-ubuntu-18.04 clang7.0.1

RUN echo 'export PATH=/clang7.0.1/bin:$PATH' >> ~/.bashrc && \
    echo 'export LD_LIBRARY_PATH=/clang7.0.1/lib:LD_LIBRARY_PATH' >> ~/.bashrc

# Install Node
RUN curl -sL https://deb.nodesource.com/setup_10.x | bash -
RUN apt-get install -y nodejs

# Install Cmake
RUN curl -L https://github.com/Kitware/CMake/releases/download/v3.14.0-rc2/cmake-3.14.0-rc2-Linux-x86_64.sh -o temp.sh && \
    chmod a+x temp.sh && \
    ./temp.sh --skip-license --prefix=${INSTALL_PREFIX}

# Install Repo
RUN git clone https://github.com/gabyx/ExecutionGraph.git 
ENV INSTALL_PREFIX="/usr/local"
ENV CHECKOUT_PATH="/ExecutionGraph"

# Start from a Bash prompt
CMD [ "/bin/bash" ]