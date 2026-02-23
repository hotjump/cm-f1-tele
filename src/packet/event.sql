-- EventUpdate
CREATE TABLE IF NOT EXISTS EventUpdate (
    ipDecimal BIGINT,
    beginUnixTime BIGINT,
    curUnixTime BIGINT,
    curTime DATETIME,
    eventIndex BIGINT,
    code CHAR(4),
    driverName VARCHAR(48),
    description VARCHAR(64),
    PRIMARY KEY (ipDecimal, beginUnixTime, curUnixTime, eventIndex),
    FOREIGN KEY (ipDecimal, beginUnixTime) REFERENCES SessionList(ipDecimal, beginUnixTime)
);

-- PenaltyUpdate
CREATE TABLE IF NOT EXISTS PenaltyUpdate (
    ipDecimal BIGINT,
    beginUnixTime BIGINT,
    curUnixTime BIGINT,
    curTime DATETIME,
    penaltyIndex BIGINT,
    code CHAR(4),
    driverName VARCHAR(48),
    otherDriverName VARCHAR(48),
    penaltyDescription VARCHAR(64),
    infringementDescription VARCHAR(64),
    timeGained INTEGER,
    lapNum INTEGER,
    placesGained INTEGER,
    PRIMARY KEY (ipDecimal, beginUnixTime, curUnixTime, penaltyIndex),
    FOREIGN KEY (ipDecimal, beginUnixTime) REFERENCES SessionList(ipDecimal, beginUnixTime)
);