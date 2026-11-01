#!/bin/bash
# ---------------------------------------------------------------
#  Whack-a-Mole - build and run
#
#  Double-click this file in Finder, or in a terminal run:
#      ./run_mac.command                 build and play
#      ./run_mac.command --standalone    also create WhackAMole-standalone.app
#                                        (runs on a Mac without Qt installed)
#      ./run_mac.command --no-run        only build
# ---------------------------------------------------------------
cd "$(dirname "$0")" || exit 1

STANDALONE=0
RUN=1
for arg in "$@"; do
    case "$arg" in
        --standalone) STANDALONE=1 ;;
        --no-run)     RUN=0 ;;
    esac
done

pause_and_exit() {
    echo
    if [ -t 0 ]; then read -r -p "Press Enter to close..." _; fi
    exit 1
}

# ---- 1. find qmake (the Qt build tool) ---------------------------------
find_qmake() {
    local name p
    for name in qmake qmake6 qmake-qt5; do
        if command -v "$name" >/dev/null 2>&1; then
            command -v "$name"
            return 0
        fi
    done
    for p in \
        /opt/homebrew/opt/qt@5/bin/qmake \
        /opt/homebrew/opt/qt/bin/qmake \
        /opt/homebrew/opt/qt@6/bin/qmake \
        /usr/local/opt/qt@5/bin/qmake \
        /usr/local/opt/qt/bin/qmake \
        /usr/local/opt/qt@6/bin/qmake \
        "$HOME"/Qt/*/macos/bin/qmake \
        "$HOME"/Qt/*/clang_64/bin/qmake \
        "$HOME"/Qt/*/*/bin/qmake ; do
        if [ -x "$p" ]; then
            echo "$p"
            return 0
        fi
    done
    return 1
}

QMAKE="$(find_qmake)"
if [ -z "$QMAKE" ]; then
    echo "Could not find Qt (qmake) on this computer."
    echo
    echo "Fix: install Qt once, then run this file again."
    echo "  - with Homebrew:   brew install qt"
    echo "  - or the installer: https://www.qt.io/download-open-source"
    echo
    echo "If you only want to PLAY and a WhackAMole-standalone.app was given"
    echo "to you, you do not need Qt: just open that app."
    pause_and_exit
fi

echo "Using Qt $("$QMAKE" -query QT_VERSION) from: $QMAKE"

# ---- 2. build -----------------------------------------------------------
JOBS="$(sysctl -n hw.ncpu 2>/dev/null || nproc 2>/dev/null || echo 2)"

"$QMAKE" WhackAMole.pro || { echo "qmake failed."; pause_and_exit; }
make -j"$JOBS"           || { echo "Build failed - see the messages above."; pause_and_exit; }

# ---- 3. make the editor understand Qt (removes red squiggles) ---------
# VS Code / Antigravity use "clangd", which needs to know where the Qt
# headers are. We copy that information from the Makefile qmake just wrote.
write_clangd() {
    [ -f Makefile ] || return 0
    local inc defs f
    inc="$(grep -E '^INCPATH[[:space:]]*=' Makefile | head -1 | sed -E 's/^INCPATH[[:space:]]*=[[:space:]]*//')"
    defs="$(grep -E '^DEFINES[[:space:]]*=' Makefile | head -1 | sed -E 's/^DEFINES[[:space:]]*=[[:space:]]*//')"
    {
        echo "CompileFlags:"
        echo "  Add:"
        echo "    - -std=c++17"
        for f in $inc $defs; do
            echo "    - \"$f\""
        done
    } > .clangd
}
write_clangd

# ---- 4. optional: standalone app ---------------------------------------
if [ "$STANDALONE" = "1" ]; then
    if [ "$(uname)" = "Darwin" ] && [ -d WhackAMole.app ]; then
        MACDEPLOY="$("$QMAKE" -query QT_INSTALL_BINS)/macdeployqt"
        if [ ! -x "$MACDEPLOY" ]; then
            echo "macdeployqt not found next to qmake ($MACDEPLOY)."
            pause_and_exit
        fi
        rm -rf WhackAMole-standalone.app
        cp -R WhackAMole.app WhackAMole-standalone.app
        "$MACDEPLOY" WhackAMole-standalone.app || { echo "macdeployqt failed."; pause_and_exit; }
        echo
        echo "Created WhackAMole-standalone.app - it runs without Qt."
        echo "To send it to someone, zip it with:"
        echo "    ditto -c -k --keepParent WhackAMole-standalone.app WhackAMole-mac.zip"
    else
        echo "--standalone only works on a Mac."
    fi
fi

# ---- 5. run -------------------------------------------------------------
if [ "$RUN" = "1" ]; then
    echo "Starting the game..."
    if [ -d WhackAMole.app ]; then
        exec ./WhackAMole.app/Contents/MacOS/WhackAMole
    else
        exec ./WhackAMole
    fi
fi
