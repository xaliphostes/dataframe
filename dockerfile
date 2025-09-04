# Use a base image with Python and C++ support
FROM python:3.11-buster

# Déclarer les arguments de construction pour les credentials GitHub
ARG GITHUB_USER
ARG GITHUB_TOKEN

# Install system dependencies
RUN sed -i 's|http://deb.debian.org/debian|http://archive.debian.org/debian|g' /etc/apt/sources.list && \
    apt-get update && apt-get install -y build-essential git && rm -rf /var/lib/apt/lists/*


# RUN apt-get update && apt-get install -y \
#     build-essential \
#     git \
#     && rm -rf /var/lib/apt/lists/*

RUN pip install cmake --upgrade

# --------------------------------------------------------------------------
# Login to Github with credentials GITHUB_USER and GITHUB_TOKEN
# export GITHUB_USER=fmaerten
# export GITHUB_TOKEN=xxxxxxxxx (see https://github.com/settings/tokens)
# --------------------------------------------------------------------------
RUN git config --global credential.helper store && \
    echo "https://${GITHUB_USER}:${GITHUB_TOKEN}@github.com" > ~/.git-credentials


# Set up working directory
WORKDIR /lib
RUN echo "=== Current working directory ===" && pwd && ls -la

# Clone the C++ source repositories
RUN git clone https://github.com/xaliphostes/dataframe.git && \
    echo "=== After cloning, current directory ===" && pwd && \
    echo "=== Directory contents ===" && ls -la

WORKDIR /lib/dataframe

# Build the C++ library
RUN mkdir build && cd build \
    && cmake .. \
    && make -j12

    RUN ctest --output-on-failure --verbose


# Clear credentials (if they were set)
RUN if [ -f ~/.git-credentials ]; then \
        rm ~/.git-credentials && \
        git config --global --unset credential.helper; \
    fi

# Set default command
CMD ["bash"]