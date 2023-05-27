#!/bin/bash

# Check if inotify is available
if [ -z "$(which inotifywait)" ]; then
    echo "inotifywait not installed."
    echo "In most distros, it is available in the inotify-tools package."
    exit 1
fi

# Function to execute on file change
function on_file_change() {
    echo "Detected change"
    make clean && make run
}
on_file_change &
# Watch for file changes in the specified directory
inotifywait --recursive --monitor --format "%e %w%f" --event modify,move,create,delete src utils | while read changed; do
    on_file_change
done
