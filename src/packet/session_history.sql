use f1_2022;

CREATE TABLE IF NOT EXISTS BestLap (
    beginUnixTime       INT UNSIGNED,
    curUnixTime         INT UNSIGNED,
    curTime             DATETIME,

    carIndex            TINYINT UNSIGNED,
    driverName          VARCHAR(48),

    bestLapTimeLapNum           TINYINT UNSIGNED,
	bestLapSector1TimeInMS      SMALLINT UNSIGNED,
	bestLapSector1TimeInStr     VARCHAR(16),
	bestLapSector2TimeInMS      SMALLINT UNSIGNED,
	bestLapSector2TimeInStr     VARCHAR(16),
	bestLapSector3TimeInMS      SMALLINT UNSIGNED,
	bestLapSector3TimeInStr     VARCHAR(16),
	bestLapTimeInMS             INT UNSIGNED,
	bestLapTimeInStr            VARCHAR(16),

	bestSector1LapNum           TINYINT UNSIGNED,
	bestSector1TimeInMS         SMALLINT UNSIGNED,
	bestSector1TimeInStr    	VARCHAR(16),
	bestSector2LapNum           TINYINT UNSIGNED,
	bestSector2TimeInMS     	SMALLINT UNSIGNED,
	bestSector2TimeInStr     	VARCHAR(16),
	bestSector3LapNum           TINYINT UNSIGNED,
	bestSector3TimeInMS      	SMALLINT UNSIGNED,
	bestSector3TimeInStr	  	VARCHAR(16),
	theoreticalBestLapTimeInMS  INT UNSIGNED,
	theoreticalBestLapTimeInStr VARCHAR(16),

    PRIMARY KEY(curUnixTime, carIndex),
    FOREIGN KEY (beginUnixTime) REFERENCES SessionList(beginUnixTime)
);

CREATE TABLE IF NOT EXISTS LapHistoryData (
	beginUnixTime       INT UNSIGNED,
    curUnixTime         INT UNSIGNED,
    curTime             DATETIME,

    carIndex            TINYINT UNSIGNED,
    driverName          VARCHAR(48),

	lapNum                      TINYINT UNSIGNED,

	sector1TimeInMS             SMALLINT UNSIGNED,
	sector1TimeInStr            VARCHAR(16),
	sector2TimeInMS             SMALLINT UNSIGNED,
	sector2TimeInStr            VARCHAR(16),
	sector3TimeInMS             SMALLINT UNSIGNED,
	sector3TimeInStr            VARCHAR(16),
	lapTimeInMS                 INT UNSIGNED,
	lapTimeInStr                VARCHAR(16),
	lapValidBitFlags        	BIT(4),

    tyreActualCompound          INT UNSIGNED,
    tyreActualCompoundInStr     CHAR(16),
    tyreVisualCompound          INT UNSIGNED,
    tyreVisualCompoundInStr     CHAR(16),
    tyreLapNumUsedInThisStint   TINYINT UNSIGNED,

    PRIMARY KEY(beginUnixTime, carIndex, lapNum),
    FOREIGN KEY (beginUnixTime) REFERENCES SessionList(beginUnixTime)
);