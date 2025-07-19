# syntax=docker/dockerfile:1
ARG GITHUB_TOKEN
# from docker-setup repo
FROM dev-env

# Make sure build‑time ARG is in scope
ARG GITHUB_TOKEN

# Switch to non‑root “dev” user (already created in dev‑env)
USER dev
WORKDIR /workspace

# 1) a-cmake-library
RUN git clone \
      https://${GITHUB_TOKEN}@github.com/knode-ai-open-source/a-cmake-library.git \
      /workspace/a-cmake-library && \
      cd /workspace/a-cmake-library && ./build_install.sh && \
      rm -rf /workspace/a-cmake-library

# 2) Build library
COPY --chown=dev:dev . /workspace/code
RUN cd /workspace/code && ./build_install.sh

# drop into a shell by default
CMD ["/bin/bash"]
