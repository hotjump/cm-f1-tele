use f1_2022;

DROP TABLE FinalClassification;
CREATE TABLE IF NOT EXISTS FinalClassification(
    beginUnixTime       INT UNSIGNED,
    curUnixTime         INT UNSIGNED,
    curTime             DATETIME,

    carIndex            TINYINT UNSIGNED,
    driverName          VARCHAR(48),
	
	teamName        VARCHAR(16), 
	position        TINYINT UNSIGNED,
	numLaps         TINYINT UNSIGNED,
	gridPosition    TINYINT UNSIGNED,
	points          TINYINT UNSIGNED,
	numPitStops     TINYINT UNSIGNED,
	resultStatus    TINYINT UNSIGNED,
	resultStatusStr VARCHAR(16),

	bestLapTime         INT UNSIGNED,
	bestLapTimeStr      VARCHAR(16),
	totalRaceTime       DOUBLE,
	totalRaceTimeStr    VARCHAR(16),
	penaltiesTime       TINYINT UNSIGNED,
	numPenalties        TINYINT UNSIGNED,
	numTyreStints       TINYINT UNSIGNED,

	tyreStintsVisual    VARCHAR(64),
	overallResult       VARCHAR(32),

    PRIMARY KEY(beginUnixTime, carIndex),
    FOREIGN KEY (beginUnixTime) REFERENCES SessionList(beginUnixTime)
);