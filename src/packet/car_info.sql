use f1_2022;
CREATE TABLE IF NOT EXISTS CarDiff (
  beginUnixTime INT UNSIGNED,
  curUnixTime INT UNSIGNED,
  curTime DATETIME,
  carIndex TINYINT UNSIGNED,
  driverName VARCHAR(48),
  carPosition TINYINT UNSIGNED,
  diffBetweenLeader FLOAT,
  diffBetweenFront FLOAT,
  diffBetweenLeaderJIT FLOAT,
  diffBetweenFrontJIT FLOAT,
  diffBetweenLastlapJIT FLOAT,
  diffBetweenBestlapJIT FLOAT,
  PRIMARY KEY(curUnixTime, carIndex),
  KEY(beginUnixTime, carIndex),
  FOREIGN KEY (beginUnixTime) REFERENCES SessionList(beginUnixTime)
);
CREATE TABLE IF NOT EXISTS CarFocus (
  beginUnixTime INT UNSIGNED,
  curUnixTime INT UNSIGNED,
  curTime DATETIME,
  carIndex TINYINT UNSIGNED,
  driverName VARCHAR(48),
  carPosition TINYINT UNSIGNED,
  scenes varchar(64),
  PRIMARY KEY(curUnixTime, carIndex),
  KEY(beginUnixTime, carIndex),
  FOREIGN KEY (beginUnixTime) REFERENCES SessionList(beginUnixTime)
);