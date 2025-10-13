FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    libssl-dev \
    zlib1g-dev \
    libjsoncpp-dev \
    uuid-dev \
    sqlite3 \
    libsqlite3-dev \
    libbrotli-dev \
    pkg-config \
    curl \
    && rm -rf /var/lib/apt/lists/*

# Install Drogon framework
RUN git clone https://github.com/drogonframework/drogon.git /tmp/drogon && \
    cd /tmp/drogon && \
    git checkout v1.9.1 && \
    git submodule update --init && \
    mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release -G Ninja && \
    ninja -j$(nproc) && \
    ninja install && \
    ldconfig && \
    rm -rf /tmp/drogon

# Set working directory
WORKDIR /app

# Copy source code (exclude build directory)
COPY CMakeLists.txt config.json config.yaml main.cc ./
COPY controllers/ controllers/
COPY models/ models/
COPY filters/ filters/
COPY plugins/ plugins/
COPY views/ views/
COPY test/ test/

# Build the application
RUN mkdir -p build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release -G Ninja && \
    ninja -j$(nproc)

# Create non-root user
RUN useradd -m -u 1000 appuser && chown -R appuser:appuser /app
USER appuser

# Expose port
EXPOSE 7777

# Health check
HEALTHCHECK --interval=30s --timeout=10s --start-period=5s --retries=3 \
    CMD curl -f http://localhost:7777/health || exit 1

# Run the application
CMD ["./build/inventory_system"]