# cm-f1-tele
Codemaster series F1 games telemetry tools and system.

## complie

linux
```
git submodule update --init
cmake .
make
```

windows
```
git submodule update --init
cmake .
minja
```

## binary file

located in bin directory.

f1-2022-tele: telemetry program for f1 2022.

distribution-test: for searching suitable random distribution.

## dependencies

dependencies: libmysqlclient libcurl-devel sqlite-devel

centos:
```
yum install libcurl-devel
yum install mysql-devel
yum install sqlite-devel
```

ubuntu
```
apt-get install libmysqlclient-dev
apt-get install sqlite3
apt-get install sqlite-devel
apt-get install -y libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
apt-get install -y libgl1-mesa-dev
```

macos
```
brew install mysql-connector-c
```

### mysql

windows: https://downloads.mysql.com/archives/installer/

## udp -> f1-tele-2022 -> mysql-server -> auto camera

f1-2022-tele listen udp data from game ,then process and store in the mysql server.CarFocus table stored current focus car when you are specting the game. You can use python script to retrive the record, then simulate keyboard input to control camera switch autoly. 

## udp -> f1-tele-2022 -> mysql-server -> grafana->server

grafana server retrive data from mysql server, then visualization in web page.

### Qualifying

![Qualifying](./pic/grafana-quali.jpeg)
### race

![race](./pic/grafana-race.jpeg)



