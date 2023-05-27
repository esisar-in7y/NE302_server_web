#!/bin/bash

# Function to execute on file change
on_file_change() {
    echo "File changed: \$1"
    make clean
    make run
}

# Monitor file changes with inotifywait
while true; do
    # inotifywait -e modify,move,create,delete --exclude obj/* .
    on_file_change "$(inotifywait -e modify,move,create,delete --exclude obj/* .)"
done
