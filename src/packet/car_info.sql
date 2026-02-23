-- CarDiff
CREATE TABLE IF NOT EXISTS CarDiff (
    ipDecimal BIGINT,
    beginUnixTime BIGINT,
    curUnixTime BIGINT,
    curTime DATETIME,
    carIndex SMALLINT,
    driverName VARCHAR(48),
    carPosition SMALLINT,
    diffBetweenLeader FLOAT,
    diffBetweenFront FLOAT,
    diffBetweenLeaderJIT FLOAT,
    diffBetweenFrontJIT FLOAT,
    diffBetweenLastlapJIT FLOAT,
    diffBetweenBestlapJIT FLOAT,
    PRIMARY KEY (ipDecimal, beginUnixTime, curUnixTime, carIndex),
    FOREIGN KEY (ipDecimal, beginUnixTime) REFERENCES SessionList(ipDecimal, beginUnixTime)
);

-- CarFocus
CREATE TABLE IF NOT EXISTS CarFocus (
    ipDecimal BIGINT,
    beginUnixTime BIGINT,
    curUnixTime BIGINT,
    curTime DATETIME,
    carIndex SMALLINT,
    driverName VARCHAR(48),
    carPosition SMALLINT,
    scenes VARCHAR(64),
    PRIMARY KEY (ipDecimal, beginUnixTime, curUnixTime, carIndex),
    FOREIGN KEY (ipDecimal, beginUnixTime) REFERENCES SessionList(ipDecimal, beginUnixTime)
);