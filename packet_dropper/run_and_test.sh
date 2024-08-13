#!/bin/bash

echo "==================== Step 1: Compile the kernel module ===================="
make
if [ $? -ne 0 ]; then
    echo "Error: Compilation failed. Exiting..."
    exit 1
fi
echo "Compilation complete."

echo "==================== Step 2: Check and unload any existing module ===================="
if lsmod | grep -q "packet_dropper"; then
    echo "Unloading existing packet_dropper module..."
    sudo rmmod packet_dropper
    if [ $? -ne 0 ]; then
        echo "Error: Failed to unload existing module. Exiting..."
        exit 1
    fi
fi
echo "Module unloaded successfully if it was loaded."

echo "==================== Step 3: Send packets without the module ===================="
ping -c 5 8.8.8.8 > ping_without_module.txt

echo "Ping output without module:"
cat ping_without_module.txt

PACKETS_RECEIVED=$(grep -oP '\d+(?= received)' ping_without_module.txt)
echo "Packets received without module: $PACKETS_RECEIVED"

if [ -z "$PACKETS_RECEIVED" ] || [ "$PACKETS_RECEIVED" -ne 5 ]; then
    echo "Error: Packet loss detected without module loaded. Exiting..."
    exit 1
fi
echo "All packets received without module."

echo "==================== Step 4: Insert the module and test packet dropping ===================="
DROP_PERCENTAGE=20  # Adjust this value as needed

echo "Inserting the packet_dropper module with $DROP_PERCENTAGE% drop..."
sudo insmod packet_dropper.ko drop_percentage=$DROP_PERCENTAGE
if [ $? -ne 0 ]; then
    echo "Error: Module insertion failed. Exiting..."
    exit 1
fi

if ! lsmod | grep -q "packet_dropper"; then
    echo "Error: Module is not loaded. Exiting..."
    exit 1
fi
echo "Module inserted successfully."

echo "Sending packets with the module..."
ping -c 5 8.8.8.8 > ping_with_module.txt

echo "Ping output with module:"
cat ping_with_module.txt

PACKETS_RECEIVED=$(grep -oP '\d+(?= received)' ping_with_module.txt)
echo "Packets received with module: $PACKETS_RECEIVED"

if [ -z "$PACKETS_RECEIVED" ] || [ "$PACKETS_RECEIVED" -eq 5 ]; then
    echo "Error: No packet loss detected with module loaded. Exiting..."
    exit 1
fi
echo "Packet loss detected as expected."

echo "==================== Step 5: Unload the module and clean up ===================="
sudo rmmod packet_dropper
if [ $? -ne 0 ]; then
    echo "Error: Failed to unload module. Exiting..."
    exit 1
fi
echo "Module unloaded successfully."

echo "Cleaning up..."
make clean
rm -f ping_without_module.txt ping_with_module.txt  # Remove ping output files

echo "==================== Test completed successfully ===================="

