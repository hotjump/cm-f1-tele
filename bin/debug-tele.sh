#!/bin/bash

nohup ./f1-2022-tele-dev --udp-port=20777 --host=127.0.0.1 --user=root --pass=123456 --db=f1_2022 --mysql-port=3306 --sql-thread-num=2 > log &
