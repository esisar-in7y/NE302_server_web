#!/bin/bash
export LD_LIBRARY_PATH=$PWD/lib
# Check if inotify is available
if [ -z "$(which inotifywait)" ]; then
    echo "inotifywait not installed."
    echo "In most distros, it is available in the inotify-tools package."
    exit 1
fi

# Initialize the process ID variable
previous_pid=
# last_execution_time=$(date +%s)
# Function to execute on file change
function on_file_change() {
    echo "Detected change"
    current_time=$(date +%s)
    time_difference=$((current_time - last_execution_time))
    # Terminate the previous process, if it exists
    if [ $time_difference -ge 2 ]; then
        last_execution_time=$(date +%s)
        if [ -n "$previous_pid" ]; then
            kill $previous_pid
        fi
        # Clean, build and run the application in the background
        make clean && make run &
        # make run &

        # Update the process ID variable
        previous_pid=$!

        # Check if 2 seconds have passed since the last execution
    
        last_execution_time=$(date +%s)
    fi
}
on_file_change
# Watch for file changes in the specified directory and trigger on_file_change function
inotifywait --recursive --monitor --format "%e %w%f" --event modify,move,create,delete src utils | while read line; do
    on_file_change
done
