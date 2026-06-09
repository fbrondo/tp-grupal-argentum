#!/bin/bash
set -euo pipefail

IMAGE_NAME="argentum-installer"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "================================================="
echo "=        ARGENTUM ONLINE INSTALLER CHECK        ="
echo "================================================="
echo ""
echo ">> Construyendo imagen base..."
docker build -t "$IMAGE_NAME" "$SCRIPT_DIR"

echo ""
echo ">> Corriendo instalador en container limpio..."
echo "   Puede tardar varios minutos (compila SDL, Qt y el proyecto)."
echo ""
docker run "$IMAGE_NAME" bash -c "
    printf 'a\nq\n' | bash /home/arguser/installer.sh &&
    test -f /home/arguser/.local/bin/taller_client &&
    test -f /home/arguser/.local/bin/taller_server &&
    test -f /home/arguser/.local/bin/taller_editor &&
    test -f /home/arguser/.local/bin/taller_launcher &&
    echo '' &&
    echo 'Instalacion verificada correctamente.'
"
