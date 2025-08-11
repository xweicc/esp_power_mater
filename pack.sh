#!/bin/bash

dd if=build/bootloader/bootloader.bin of=$1.bin bs=1K
dd if=build/partition_table/partition-table.bin of=$1.bin bs=1K seek=32
dd if=build/xt60_power_meter.bin of=$1.bin bs=1K seek=64
