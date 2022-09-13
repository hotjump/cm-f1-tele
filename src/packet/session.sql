use f1_2022;

CREATE TABLE IF NOT EXISTS SessionList (
    beginUnixTime               INT UNSIGNED PRIMARY KEY,
    lastUnixTime                INT UNSIGNED,

    beginTime                   DATETIME,
    sessionTime                 FLOAT,
    sessionUID                  BIGINT UNSIGNED,
    frameIdentifier             INT UNSIGNED,
	packetFormat                INT UNSIGNED,
	gameMajorVersion            INT UNSIGNED,
	gameMinorVersion            INT UNSIGNED,
    packetVersion               INT UNSIGNED,
    playerCarIndex              INT UNSIGNED,
    secondaryPlayerCarIndex     INT UNSIGNED
);

CREATE TABLE IF NOT EXISTS SessionData (
    beginUnixTime               INT UNSIGNED,
    curUnixTime                 INT UNSIGNED,
    curTime                     DATETIME,

    weather                     TINYINT UNSIGNED,
    weatherInStr                VARCHAR(16),
	trackTemperature            TINYINT,
	airTemperature              TINYINT,		
	totalLaps                   TINYINT UNSIGNED,		
	trackLength                 SMALLINT UNSIGNED,		
	sessionType                 TINYINT UNSIGNED,
    sessionTypeInStr            VARCHAR(16),
	trackId                     TINYINT,
    trackIdInStr                VARCHAR(20),
	formula                     TINYINT UNSIGNED,
    formulaInStr                VARCHAR(16),
	sessionTimeLeft             SMALLINT UNSIGNED,		
	sessionDuration             SMALLINT UNSIGNED,		
	pitSpeedLimit               TINYINT UNSIGNED,	
	gamePaused                  TINYINT UNSIGNED,
	isSpectating                TINYINT UNSIGNED,
	spectatorCarIndex           TINYINT UNSIGNED,
	sliProNativeSupport         TINYINT UNSIGNED,
	numMarshalZones             TINYINT UNSIGNED,
	safetyCarStatus             TINYINT UNSIGNED,
    safetyCarStatusInStr        VARCHAR(16),
	networkGame                 TINYINT UNSIGNED,
    numWeatherForecastSamples   TINYINT UNSIGNED,
    forecastAccuracy            TINYINT UNSIGNED,                   
    aiDifficulty                TINYINT UNSIGNED,                     
    seasonLinkIdentifier        INT UNSIGNED,                       
    weekendLinkIdentifier       INT UNSIGNED,                     
    sessionLinkIdentifier       INT UNSIGNED,                    
    pitStopWindowIdealLap       TINYINT UNSIGNED,                   
    pitStopWindowLatestLap      TINYINT UNSIGNED,                   
    pitStopRejoinPosition       TINYINT UNSIGNED,                 
    steeringAssist              TINYINT UNSIGNED,                  
    brakingAssist               TINYINT UNSIGNED,                    
    gearboxAssist               TINYINT UNSIGNED,                   
    pitAssist                   TINYINT UNSIGNED,                   
    pitReleaseAssist            TINYINT UNSIGNED,                   
    ERSAssist                   TINYINT UNSIGNED,                    
    DRSAssist                   TINYINT UNSIGNED,                    
    dynamicRacingLine           TINYINT UNSIGNED,                 
    dynamicRacingLineType       TINYINT UNSIGNED,                   
    gameMode                    TINYINT UNSIGNED,
    gameModeInStr               VARCHAR(16),                  
    ruleSet                     TINYINT UNSIGNED,
    ruleSetInStr                VARCHAR(24),                   
    timeOfDay                   INT UNSIGNED,                   
    sessionLength               TINYINT UNSIGNED,
    sessionLengthInStr          VARCHAR(16),              

    PRIMARY KEY(curUnixTime),
    KEY(beginUnixTime, curUnixTime),
    FOREIGN KEY (beginUnixTime) REFERENCES SessionList(beginUnixTime)
);

CREATE TABLE IF NOT EXISTS WeatherForecast (
    beginUnixTime               INT UNSIGNED,
    curUnixTime                 INT UNSIGNED,
    curTime                     DATETIME,

    timeOffset                  INT UNSIGNED,
    sessionType                 TINYINT UNSIGNED,
    sessionTypeInStr            VARCHAR(16), 
    weather                     TINYINT UNSIGNED,
    weatherInStr                VARCHAR(16),

    trackTemperature            TINYINT,
    trackTemperatureChange      TINYINT,
    trackTemperatureChangeInStr VARCHAR(16),
    airTemperature              TINYINT,
    airTemperatureChange        TINYINT,
    airTemperatureChangeInStr   VARCHAR(16),
    rainPercentage              TINYINT UNSIGNED,

    PRIMARY KEY(curUnixTime, timeOffset, sessionType),
    KEY(beginUnixTime, curUnixTime, timeOffset),
    FOREIGN KEY (beginUnixTime) REFERENCES SessionList(beginUnixTime)
);

CREATE TABLE IF NOT EXISTS MarshalZones (
    beginUnixTime           INT UNSIGNED,
    curUnixTime             INT UNSIGNED,
    curTime                 DATETIME,

    zoneIndex               INT UNSIGNED,
    zoneStart               FLOAT,
    zoneFlag                TINYINT,
    zoneFlagInStr           VARCHAR(16),

    PRIMARY KEY(curUnixTime, zoneIndex),
    KEY(beginUnixTime, curUnixTime, zoneIndex),
    FOREIGN KEY (beginUnixTime) REFERENCES SessionList(beginUnixTime)
);