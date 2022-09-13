use f1_2022;

CREATE TABLE IF NOT EXISTS Participants (
    beginUnixTime       INT UNSIGNED,
    curUnixTime         INT UNSIGNED,
    curTime             DATETIME,

    carIndex            TINYINT UNSIGNED,
    driverName          VARCHAR(48),

    aiControlled    TINYINT UNSIGNED,
	driverId        TINYINT UNSIGNED,
    networkId       TINYINT UNSIGNED,
	teamId          TINYINT UNSIGNED,
    teamName        VARCHAR(48),
    myTeam          TINYINT UNSIGNED,
	raceNumber      TINYINT UNSIGNED,
	nationality     TINYINT UNSIGNED,
    nationalityName VARCHAR(20),
	yourTelemetry   TINYINT UNSIGNED,

    PRIMARY KEY(curUnixTime, carIndex),
    KEY(beginUnixTime, carIndex),
    FOREIGN KEY (beginUnixTime) REFERENCES SessionList(beginUnixTime)
);
