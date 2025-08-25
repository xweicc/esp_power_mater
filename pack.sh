#!/bin/bash

# Check if parameter exists
if [ -z "$1" ]; then
    echo "Error: Please provide output filename parameter"
    echo "Usage: $0 <output_filename>"
    exit 1
fi

dd if=build/bootloader/bootloader.bin of=$1.bin bs=1K
dd if=build/partition_table/partition-table.bin of=$1.bin bs=1K seek=32
dd if=build/xt60_power_meter.bin of=$1.bin bs=1K seek=64

echo "Output file: $1.bin"