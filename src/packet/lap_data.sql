use f1_2022;

CREATE TABLE IF NOT EXISTS LapData (
    beginUnixTime       INT UNSIGNED,
    curUnixTime         INT UNSIGNED,
    curTime             DATETIME,

    carIndex            TINYINT UNSIGNED,
    driverName          VARCHAR(48),
	teamName	        VARCHAR(16), 

    lastLapTimeInMS     INT UNSIGNED,
	lastLapTimeInStr    VARCHAR(16),
	currentLapTimeInMS  INT UNSIGNED,
    currentLapTimeInStr VARCHAR(16),
	sector1TimeInMS     SMALLINT UNSIGNED,
	sector1TimeInStr    VARCHAR(16),
	sector2TimeInMS     SMALLINT UNSIGNED,
	sector2TimeInStr    VARCHAR(16),

	lapDistance         FLOAT,
	totalDistance       FLOAT,
	safetyCarDelta      FLOAT,
	carPosition         TINYINT UNSIGNED,
	currentLapNum       TINYINT UNSIGNED,
	pitStatus           TINYINT UNSIGNED,
    pitStatusChar       VARCHAR(12),
    numPitStops         TINYINT UNSIGNED,
	sector              TINYINT UNSIGNED,
	currentLapInvalid   TINYINT UNSIGNED,
	penalties           TINYINT UNSIGNED,
    warnings            TINYINT UNSIGNED,
    numUnservedDriveThroughPens TINYINT UNSIGNED,
    numUnservedStopGoPens       TINYINT UNSIGNED,
	gridPosition        TINYINT UNSIGNED,
	driverStatus        TINYINT UNSIGNED,
    driverStatusChar    VARCHAR(10),
	resultStatus        TINYINT UNSIGNED,
    resultStatusChar    VARCHAR(16),

    pitLaneTimerActive  	TINYINT UNSIGNED,
    pitLaneTimeInLaneInMS     SMALLINT UNSIGNED,
    pitStopTimerInMS          SMALLINT UNSIGNED,
    pitStopShouldServePen     TINYINT UNSIGNED,

    PRIMARY KEY(curUnixTime, carIndex),
    KEY(beginUnixTime, carIndex),
    FOREIGN KEY (beginUnixTime) REFERENCES SessionList(beginUnixTime)
);