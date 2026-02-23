CREATE TABLE IF NOT EXISTS LobbyInfo (
    ipDecimal BIGINT,
    curUnixTime BIGINT,
    curTime DATETIME,
    carIndex SMALLINT,
    driverName VARCHAR(48),
    aiControlled SMALLINT,
    teamId SMALLINT,
    teamName VARCHAR(16),
    nationality SMALLINT,
    carNumber SMALLINT,
    readyStatus SMALLINT,
    readyStatusStr VARCHAR(16),
    PRIMARY KEY (ipDecimal, curUnixTime, carIndex),
    FOREIGN KEY (ipDecimal) REFERENCES IpList(ipDecimal)
);