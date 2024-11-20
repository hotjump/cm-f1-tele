CREATE TABLE IF NOT EXISTS EventUpdate (
	ipDecimal INT UNSIGNED,
	beginUnixTime INT UNSIGNED,
	curUnixTime INT UNSIGNED,
	curTime DATETIME,
	eventIndex INT UNSIGNED,
	code CHAR(4),
	driverName VARCHAR(48),
	description VARCHAR(64),
	PRIMARY KEY(
		ipDecimal,
		beginUnixTime,
		curUnixTime,
		eventIndex
	),
	FOREIGN KEY(ipDecimal, beginUnixTime) REFERENCES SessionList(ipDecimal, beginUnixTime) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS PenaltyUpdate (
	ipDecimal INT UNSIGNED,
	beginUnixTime INT UNSIGNED,
	curUnixTime INT UNSIGNED,
	curTime DATETIME,
	penaltyIndex INT UNSIGNED,
	code CHAR(4),
	driverName VARCHAR(48),
	otherDriverName VARCHAR(48),
	penaltyDescription VARCHAR(64),
	infringementDescription VARCHAR(64),
	timeGained INT UNSIGNED,
	lapNum INT UNSIGNED,
	placesGained INT UNSIGNED,
	PRIMARY KEY(
		ipDecimal,
		beginUnixTime,
		curUnixTime,
		penaltyIndex
	),
	FOREIGN KEY(ipDecimal, beginUnixTime) REFERENCES SessionList(ipDecimal, beginUnixTime) ON DELETE CASCADE
);