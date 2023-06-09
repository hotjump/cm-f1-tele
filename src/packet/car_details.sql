USE f1_2022_tele;

CREATE TABLE IF NOT EXISTS CarDetails (
    ipDecimal           INT UNSIGNED,
    beginUnixTime       INT UNSIGNED,
    curUnixTime         INT UNSIGNED,
    frameIdentifier     INT UNSIGNED,
    curTime             DATETIME,

    carIndex            TINYINT UNSIGNED,
    driverName          VARCHAR(48),

    speed               SMALLINT UNSIGNED,                   
    throttle            FLOAT,                  
    steer               FLOAT,                 
    brake               FLOAT,                   
    clutch              TINYINT UNSIGNED,                    
    gear                TINYINT,                       
    engineRPM           SMALLINT UNSIGNED,                
    drs                 TINYINT UNSIGNED,                      

    worldPositionX      FLOAT,
    worldPositionY      FLOAT,
    worldPositionZ      FLOAT,

    currentLapTimeInMS  INT UNSIGNED,
    currentLapTimeInStr VARCHAR(16),
	sector1TimeInMS     SMALLINT UNSIGNED,
	sector1TimeInStr    VARCHAR(16),
	sector2TimeInMS     SMALLINT UNSIGNED,
	sector2TimeInStr    VARCHAR(16),

	lapDistance         FLOAT,
	totalDistance       FLOAT,
    carPosition         TINYINT UNSIGNED,
	currentLapNum       TINYINT UNSIGNED,

    PRIMARY KEY(ipDecimal, beginUnixTime, curUnixTime, frameIdentifier, carIndex),
    FOREIGN KEY(ipDecimal, beginUnixTime) REFERENCES SessionList(ipDecimal, beginUnixTime) ON DELETE CASCADE
);