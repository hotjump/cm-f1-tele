CREATE TABLE IF NOT EXISTS CarDiff (
  ipDecimal INT UNSIGNED,
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
  PRIMARY KEY(ipDecimal, beginUnixTime, curUnixTime, carIndex),
  FOREIGN KEY(ipDecimal, beginUnixTime) REFERENCES SessionList(ipDecimal, beginUnixTime) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS CarFocus (
  ipDecimal INT UNSIGNED,
  beginUnixTime INT UNSIGNED,
  curUnixTime INT UNSIGNED,
  curTime DATETIME,
  carIndex TINYINT UNSIGNED,
  driverName VARCHAR(48),
  carPosition TINYINT UNSIGNED,
  scenes varchar(64),
  PRIMARY KEY(ipDecimal, beginUnixTime, curUnixTime, carIndex),
  FOREIGN KEY(ipDecimal, beginUnixTime) REFERENCES SessionList(ipDecimal, beginUnixTime) ON DELETE CASCADE
);