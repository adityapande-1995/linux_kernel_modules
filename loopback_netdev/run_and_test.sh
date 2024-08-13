#!/bin/bash

set -e  # Exit immediately if a command exits with a non-zero status
set -x  # Print commands and their arguments as they are executed

MODULE_NAME="loopback_netdev"
INTERFACE_NAME="loopback0"

# Clean previous build artifacts
echo "Cleaning up previous build..."
make clean

# Build the kernel module
echo "Building the kernel module..."
make

# Check if the interface already exists and remove the module if it does
if ip link show | grep -q "$INTERFACE_NAME"; then
    echo "Interface $INTERFACE_NAME already exists. Removing module..."
    sudo rmmod $MODULE_NAME
fi

# Insert the module
echo "Loading the kernel module..."
sudo insmod ./$MODULE_NAME.ko

# Check kernel logs for any debug messages
echo "Checking kernel logs for debug messages..."
dmesg | tail -n 20

# Verify that the interface has been created
echo "Verifying that the interface $INTERFACE_NAME has been created..."
if ifconfig -a | grep -q "$INTERFACE_NAME"; then
    echo "Interface $INTERFACE_NAME has been created successfully."
else
    echo "Error: Interface $INTERFACE_NAME was not created."
    exit 1
fi

# List all network interfaces for verification
echo "List of all network interfaces:"
ifconfig -a

# Bring up the loopback0 interface and assign an IP address
echo "Bringing up the $INTERFACE_NAME interface and assigning an IP address..."
sudo ifconfig $INTERFACE_NAME 127.0.0.2 netmask 255.0.0.0 up

# Verify that the interface is up
echo "Verifying that $INTERFACE_NAME is up..."
if ifconfig $INTERFACE_NAME | grep -q "inet 127.0.0.2"; then
    echo "$INTERFACE_NAME is up and configured with IP address 127.0.0.2."
else
    echo "Error: Failed to bring up $INTERFACE_NAME."
    exit 1
fi

# Pinging through the loopback0 interface
echo "Pinging through $INTERFACE_NAME..."
ping -I $INTERFACE_NAME -c 4 127.0.0.2

# Clean up by removing the module
echo "Removing the kernel module..."
sudo rmmod $MODULE_NAME

echo "Test completed successfully."

