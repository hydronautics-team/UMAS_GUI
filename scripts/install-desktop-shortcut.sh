#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
PROJECT_ROOT="$(realpath "$SCRIPT_DIR/..")"
DESKTOP_DIR="$(xdg-user-dir DESKTOP 2>/dev/null || true)"

if [[ -z "$DESKTOP_DIR" ]]; then
    DESKTOP_DIR="$HOME/Desktop"
fi

mkdir -p "$DESKTOP_DIR"

desktop-file-install \
    --dir="$DESKTOP_DIR" \
    --set-key=Exec \
    --set-value="$PROJECT_ROOT/scripts/umas-gui run" \
    --set-key=Path \
    --set-value="$PROJECT_ROOT" \
    --set-icon="$PROJECT_ROOT/resources/images/hydronautics_logo.png" \
    "$PROJECT_ROOT/resources/umas-gui.desktop"

SHORTCUT="$DESKTOP_DIR/umas-gui.desktop"
chmod +x "$SHORTCUT"

if command -v gio >/dev/null 2>&1; then
    if ! gio set "$SHORTCUT" metadata::trusted true; then
        echo "Warning: GNOME could not mark the shortcut as trusted." >&2
        echo "Right-click it and select 'Allow Launching'." >&2
    fi
fi

echo "Installed desktop shortcut: $SHORTCUT"
