#!/bin/sh
#
# Script run by the container.
#
# Copy default configuration files into /usr/local/etc/crossfire
# if they are not already present, then start the server.

set -e

EXAMPLE_DIR=/usr/local/share/crossfire-example
CONFIG_DIR=/usr/local/etc/crossfire

if [ -d "$EXAMPLE_DIR" ]; then
    for src in "$EXAMPLE_DIR"/*; do
        dest="$CONFIG_DIR/$(basename "$src")"
        if [ ! -e "$dest" ]; then
            cp -r "$src" "$dest"
        fi
    done
fi

exec /usr/local/bin/crossfire-server "$@"
