#!/bin/bash

# Run the telemetry binary (relative to this script)
echo "Starting telemetry binary..."
./f1-2022-tele-dev --udp-port=20777 --host=127.0.0.1 --user=telemetry --pass=pass@tele --db=f1_2022_tele --mysql-port=3307 --sql-thread-num=2

