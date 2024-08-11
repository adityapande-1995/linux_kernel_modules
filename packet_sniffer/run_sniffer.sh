#!/bin/bash

MODULE_NAME="packet_sniffer"
USER_APP="./build/sniffer_display"
MODULE_FILE="./packet_sniffer.ko"
LOG_FILE="/tmp/sniffer_output.log"

# Function to clean up
cleanup() {
    echo "Cleaning up..."
    sudo rmmod $MODULE_NAME 2>/dev/null
    sudo dmesg -C 2>/dev/null  # Clear dmesg to avoid old logs
    rm -f $LOG_FILE
    make clean
    echo "Cleanup complete."
}

# Function to build everything
build() {
    echo "Building kernel module and user-space application..."
    make || exit 1
    echo "Build complete."
}

# Function to run the sniffer
run_sniffer() {
    echo "Loading kernel module..."
    sudo insmod $MODULE_FILE || exit 1
    echo "Kernel module loaded."

    echo "Starting user-space application..."
    $USER_APP | tee $LOG_FILE &
    USER_APP_PID=$!

    echo "Sniffer running. You can now test with commands like 'ping google.com' or 'curl google.com'."
    echo "Press any key to stop..."
    read -n 1 -s

    echo "Stopping sniffer..."
    kill $USER_APP_PID
    sudo rmmod $MODULE_NAME
    echo "Sniffer stopped."
}

# Start script
case "$1" in
    clean)
        cleanup
        ;;
    *)
        cleanup
        build
        run_sniffer
        cleanup
        ;;
esac

