FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

# Instalar git y sudo
RUN apt-get update && \
    apt-get install -y --no-install-recommends git sudo ca-certificates && \
    rm -rf /var/lib/apt/lists/*

# Crear usuario no root con sudo sin contraseña (simula entorno real de usuario)
RUN useradd -m -s /bin/bash arguser && \
    echo "arguser ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

USER arguser
WORKDIR /home/arguser

COPY --chown=arguser:arguser installer.sh ./installer.sh
RUN chmod +x installer.sh
