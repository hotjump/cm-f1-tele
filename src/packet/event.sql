use f1_2022;

CREATE TABLE IF NOT EXISTS EventUpdate (
    beginUnixTime       INT UNSIGNED,
    curUnixTime         INT UNSIGNED,
    curTime             DATETIME,

	eventIndex 			INT UNSIGNED,

	code                CHAR(4),
	driverName          VARCHAR(48),
	description         VARCHAR(64),
    
	PRIMARY KEY(curUnixTime, eventIndex),
    KEY(beginUnixTime, curUnixTime),
    FOREIGN KEY (beginUnixTime) REFERENCES SessionList(beginUnixTime)
);

CREATE TABLE IF NOT EXISTS PenaltyUpdate (
    beginUnixTime       INT UNSIGNED,
    curUnixTime         INT UNSIGNED,
    curTime             DATETIME,

	penaltyIndex 		INT UNSIGNED,

	code                CHAR(4),
	driverName          VARCHAR(48),
	otherDriverName     VARCHAR(48),
	penaltyDescription  VARCHAR(64),
	infringementDescription VARCHAR(64),
	timeGained          INT UNSIGNED, 
	lapNum              INT UNSIGNED,
	placesGained        INT UNSIGNED,
    
	PRIMARY KEY(curUnixTime, penaltyIndex),
    KEY(beginUnixTime, curUnixTime),
    FOREIGN KEY (beginUnixTime) REFERENCES SessionList(beginUnixTime)
);