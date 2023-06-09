#!/bin/bash

nohup ./f1-2022-tele --udp-port=20777 --host=127.0.0.1 --user=telemetry --pass=afr@tele --db=f1_2022_tele --mysql-port=3306 --sql-thread-num=2 > log &
