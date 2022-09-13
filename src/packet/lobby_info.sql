use f1_2022;

CREATE TABLE IF NOT EXISTS LobbyInfo(
    curUnixTime         INT UNSIGNED,
    curTime             DATETIME,

    carIndex            TINYINT UNSIGNED,
    driverName          VARCHAR(48),

	aiControlled      TINYINT UNSIGNED,
    teamId            TINYINT UNSIGNED,
	teamName          VARCHAR(16),
    nationality       TINYINT UNSIGNED,
    carNumber         TINYINT UNSIGNED,
	
	readyStatus       TINYINT UNSIGNED,
	readyStatusStr    VARCHAR(16),

	PRIMARY KEY(curUnixTime, carIndex)
);