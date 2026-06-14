#!/bin/bash
set -euo pipefail

# ---- Configuración 
APP_NAME="argentum"
REPO_URL="https://github.com/fbrondo/tp-grupal-argentum.git"
SHARE_DIR="${HOME}/.local/share/${APP_NAME}"
CONFIG_DIR="${HOME}/.config/${APP_NAME}"
BIN_DIR="${HOME}/.local/bin"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

apt_get() {
    if [ "$(id -u)" -eq 0 ]; then
        apt-get "$@"
    else
        sudo apt-get "$@"
    fi
}

# ---- Helpers 
function header() { echo ""; echo "=== $* ==="; echo ""; }
function info()   { echo "  >> $*"; }

# Detecta si el script corre desde dentro del repositorio; si no, lo clona.
# Resultado en la variable global REPO_DIR.
REPO_DIR="${HOME}/tp-grupal-argentum"
function resolveRepoDir() {
    if git -C "$SCRIPT_DIR" rev-parse --git-dir &>/dev/null; then
        REPO_DIR="$(git -C "$SCRIPT_DIR" rev-parse --show-toplevel)"
    elif [ -d "${REPO_DIR}/.git" ]; then
        info "Repositorio ya clonado en ${REPO_DIR}, reutilizando."
    else
        info "Clonando repositorio en ${REPO_DIR}..."
        git clone "$REPO_URL" "$REPO_DIR"
    fi
}

# ---- Dependencias 
function installDependencies() {
    header "INSTALACIÓN DE DEPENDENCIAS"
    apt_get update -y
    apt_get install -y \
        cmake build-essential git ca-certificates \
        libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev \
        libopusfile-dev libxmp-dev \
        libjack-jackd2-dev libfluidsynth-dev fluidsynth \
        libwavpack-dev libmodplug-dev libvorbis-dev libflac-dev \
        libmpg123-dev libogg-dev libfreetype6-dev libharfbuzz-dev \
        libasound2-dev libpulse-dev \
        libgl-dev qt6-base-dev qt6-tools-dev
    info "Dependencias instaladas."
}

# ---- Compilación 
function buildProject() {
    local repo_dir="$1"
    local build_dir="${repo_dir}/build"

    header "COMPILACIÓN"
    cmake -S "$repo_dir" -B "$build_dir" \
        -DARGENTUM_SHARE_PATH="${SHARE_DIR}" \
        -DARGENTUM_CONFIG_PATH="${CONFIG_DIR}/server/config" \
        -DTALLER_LAUNCHER=ON \
        -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON \
        -DCMAKE_INSTALL_RPATH="\$ORIGIN/../lib"
    cmake --build "$build_dir" -j"$(nproc)"
    info "Compilación finalizada."
}

# ---- Tests 
function runTests() {
    local repo_dir="$1"
    header "TESTS"
    (cd "$repo_dir" && "./build/taller_tests")
    info "Tests pasados."
}

# ---- Instalación de archivos 
function installFiles() {
    local repo_dir="$1"
    local build_dir="${repo_dir}/build"

    header "INSTALACIÓN DE ARCHIVOS"

    local lib_dir="${HOME}/.local/lib"
    mkdir -p "$BIN_DIR" "$lib_dir" \
             "${SHARE_DIR}/client" \
             "${SHARE_DIR}/common" \
             "${CONFIG_DIR}/server/config/data"

    # Shared libraries → ~/.local/lib/
    find "$build_dir" -name "*.so*" ! -path "*/CMakeFiles/*" -exec cp -Pf {} "$lib_dir/" \;
    info "Librerías: ${lib_dir}"

    # Binarios → ~/.local/bin/
    for bin in taller_client taller_server taller_editor taller_launcher; do
        if [ -f "${build_dir}/${bin}" ]; then
            cp "${build_dir}/${bin}" "${BIN_DIR}/"
            info "Binario: ${BIN_DIR}/${bin}"
        fi
    done

    # Assets del cliente → ~/.local/share/argentum/client/assets/
    cp -r "${repo_dir}/client/assets" "${SHARE_DIR}/client/"

    # Assets compartidos → ~/.local/share/argentum/common/assets/ y common/data/
    cp -r "${repo_dir}/common/assets" "${SHARE_DIR}/common/"
    cp -r "${repo_dir}/common/data"   "${SHARE_DIR}/common/"

    # Config del servidor → ~/.config/argentum/server/config/
    cp -r "${repo_dir}/server/config/." "${CONFIG_DIR}/server/config/"

    info "Assets:  ${SHARE_DIR}"
    info "Config:  ${CONFIG_DIR}/server/config"
    info "Binarios: ${BIN_DIR}"
}

# ---- Instalación completa del juego 
function installGame() {
    resolveRepoDir
    buildProject "$REPO_DIR"
    # runTests "$REPO_DIR"
    installFiles "$REPO_DIR"
    header "INSTALACIÓN COMPLETA"
    info "Ejecutar el servidor: taller_server <puerto>"
    info "Ejecutar el editor:   taller_editor"
    info "Ejecutar el cliente:  taller_client <host> <puerto>"
    info "(Asegurese de que ${BIN_DIR} esté en su PATH)"
}

# ---- Desinstalación 
function uninstall() {
    header "DESINSTALACIÓN"
    rm -f "${BIN_DIR}/taller_client" "${BIN_DIR}/taller_server" "${BIN_DIR}/taller_editor"
    rm -rf "${SHARE_DIR}" "${CONFIG_DIR}"
    info "Juego desinstalado."
}

# ---- Menú 
function helpMessage() {
    echo "Opciones de instalación:"
    echo "  d  instalar dependencias del sistema (SDL, Qt6, etc.)"
    echo "  i  compilar e instalar el juego (requiere dependencias previas)"
    echo "  a  todo: dependencias + compilar + instalar"
    echo ""
    echo "Opciones de desinstalación:"
    echo "  u  desinstalar el juego"
    echo ""
    echo "Otras opciones:"
    echo "  h  esta ayuda"
    echo "  q  salir"
    echo ""
}

[ -t 1 ] && clear
echo "================================================="
echo "=          ARGENTUM ONLINE INSTALLER            ="
echo "================================================="
echo ""
helpMessage
printf "> Ingrese una opción: "

while true; do
    IFS= read -r OPTION || { echo ""; echo "Adiós!"; exit 0; }
    echo ""
    case "$OPTION" in
        d) installDependencies;               printf "> Ingrese una opción: " ;;
        i) installGame;                       printf "> Ingrese una opción: " ;;
        a) installDependencies; installGame;  printf "> Ingrese una opción: " ;;
        u) uninstall;                         printf "> Ingrese una opción: " ;;
        h) helpMessage;                       printf "> Ingrese una opción: " ;;
        q) echo "Adiós!"; exit 0 ;;
        *) echo "Opción desconocida (h para ayuda)."; printf "> Ingrese una opción: " ;;
    esac
done
