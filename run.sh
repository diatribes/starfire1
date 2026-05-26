#!/usr/bin/env bash
# Launch the demo under Wine on Linux, auto-selecting the GPU's EGL ICD so the
# window isn't black. Wine 11 drives OpenGL through EGL, and libglvnd has to load
# the real vendor driver (e.g. NVIDIA) rather than Mesa. See README "Gotchas".
#   ./run.sh [exe]    default: starfire1.exe
cd "$(dirname "$0")"

exe="${1:-starfire1.exe}"
[ -f "$exe" ] || { echo "run.sh: '$exe' not found (build it first)"; exit 1; }

export WINEPREFIX="${WINEPREFIX:-$HOME/.wine-starfire}"
export WINEDLLOVERRIDES="${WINEDLLOVERRIDES:-mscoree,mshtml=}"
export WINEDEBUG="${WINEDEBUG:--all}"

# Pick an EGL ICD only if one isn't already chosen: if an NVIDIA EGL manifest
# exists (driver installed), use it; otherwise leave libglvnd to its default.
# On a hybrid/Optimus laptop where the iGPU drives the display, forcing NVIDIA
# may be wrong; export __EGL_VENDOR_LIBRARY_FILENAMES yourself to override.
if [ -z "${__EGL_VENDOR_LIBRARY_FILENAMES:-}" ]; then
  shopt -s nullglob
  for f in /usr/share/glvnd/egl_vendor.d/*nvidia*.json \
           /etc/glvnd/egl_vendor.d/*nvidia*.json; do
    export __EGL_VENDOR_LIBRARY_FILENAMES="$f"
    break
  done
fi

echo "[run] $exe  (prefix=$WINEPREFIX, icd=${__EGL_VENDOR_LIBRARY_FILENAMES:-default})"
exec wine "$exe"
