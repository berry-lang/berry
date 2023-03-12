#!/bin/bash
set -eu

remake() {
    echo "--- Remaking HTML -------------------------------"
    pushd docs >/dev/null
    make html
    popd >/dev/null
}

remake
python3 -m http.server -d docs/build/html 8085 &

while inotifywait -r -e modify,create,delete,move .; do
    remake
done
