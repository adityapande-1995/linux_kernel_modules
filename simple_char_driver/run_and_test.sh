#!/bin/bash

MODULE_NAME="simple_char_driver"
DEVICE_NAME="/dev/$MODULE_NAME"
LOG_FILE="/tmp/kernel_module_test.log"
PRINTK_MESSAGES=("Initializing the SimpleChar LKM" "Device has been opened" "Writing to the device" "Reading from the device" "Device successfully closed" "Goodbye from the LKM!")

# Function to exit if any command fails
exit_on_failure() {
    echo "$1"
    exit 1
}

# Cleanup any previous instance of the module
echo "Cleaning up any previous instance of the module..."
if lsmod | grep -q "$MODULE_NAME"; then
    sudo rmmod $MODULE_NAME || exit_on_failure "Failed to remove existing module $MODULE_NAME. Exiting."
fi
make clean || exit_on_failure "Failed to clean previous build files. Exiting."

# Build the kernel module
echo "Building the kernel module..."
make || exit_on_failure "Module build failed. Exiting."

# Check if the module build was successful
if [ ! -f "${MODULE_NAME}.ko" ]; then
    exit_on_failure "Module build failed. Exiting."
fi

# Load the module
echo "Loading the kernel module..."
sudo insmod ./${MODULE_NAME}.ko || exit_on_failure "Failed to load module $MODULE_NAME. Exiting."

# Verify that the module is loaded
if lsmod | grep -q "$MODULE_NAME"; then
    echo "Module $MODULE_NAME loaded successfully."
else
    exit_on_failure "Failed to load module $MODULE_NAME. Exiting."
fi

# Interact with the device file to generate the missing printk messages
if [ -e "$DEVICE_NAME" ]; then
    echo "Interacting with the device file $DEVICE_NAME..."
    sudo cat "$DEVICE_NAME" > /dev/null  # Trigger read operation
    echo "Hello" | sudo tee "$DEVICE_NAME" > /dev/null  # Trigger write operation
    sudo cat "$DEVICE_NAME" > /dev/null  # Trigger another read operation
else
    exit_on_failure "Device file $DEVICE_NAME does not exist. Exiting."
fi

# Check the kernel log for specific printk messages
echo "Checking the kernel log for printk messages..."
sudo dmesg | tail -n 100 > $LOG_FILE || exit_on_failure "Failed to write dmesg output to log file."

all_found=true
for message in "${PRINTK_MESSAGES[@]}"; do
    if grep -q "$message" $LOG_FILE; then
        echo "Found expected printk message: '$message'"
    else
        echo "Did not find expected printk message: '$message'"
        all_found=false
    fi
done

if $all_found; then
    echo "All expected printk messages were found."
else
    exit_on_failure "Some expected printk messages were missing. Exiting."
fi

# Remove the module
echo "Removing the kernel module..."
sudo rmmod $MODULE_NAME || exit_on_failure "Failed to remove module $MODULE_NAME. Exiting."

# Clean up build files
echo "Cleaning up build files..."
make clean || exit_on_failure "Failed to clean build files. Exiting."

# Final message
echo "Test script completed successfully."

