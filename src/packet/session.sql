DROP DATABASE IF EXISTS f1_2022_tele;
CREATE DATABASE IF NOT EXISTS f1_2022_tele;

USE f1_2022_tele;

CREATE TABLE IF NOT EXISTS IpList (
    ipDecimal                   INT UNSIGNED PRIMARY KEY,
    updateUnixTime              INT UNSIGNED,
    updateTime                  DATETIME,
    ipString                    VARCHAR(20),
    ipComeFrom                  JSON
);

CREATE TABLE IF NOT EXISTS SessionList (
    ipDecimal                   INT UNSIGNED,
    beginUnixTime               INT UNSIGNED,
    curUnixTime                 INT UNSIGNED,

    beginTime                   DATETIME,
    sessionTime                 FLOAT,
    sessionUID                  BIGINT UNSIGNED,
    frameIdentifier             INT UNSIGNED,
	packetFormat                INT UNSIGNED,
	gameMajorVersion            INT UNSIGNED,
	gameMinorVersion            INT UNSIGNED,
    packetVersion               INT UNSIGNED,
    playerCarIndex              INT UNSIGNED,
    secondaryPlayerCarIndex     INT UNSIGNED,
    PRIMARY KEY(ipDecimal, beginUnixTime),
    FOREIGN KEY(ipDecimal) REFERENCES IpList(ipDecimal) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS SessionData (
    ipDecimal                   INT UNSIGNED,
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

    PRIMARY KEY(ipDecimal, beginUnixTime, curUnixTime),
    FOREIGN KEY(ipDecimal, beginUnixTime) REFERENCES SessionList(ipDecimal, beginUnixTime) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS WeatherForecast (
    ipDecimal                   INT UNSIGNED,
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

    PRIMARY KEY(ipDecimal, beginUnixTime, curUnixTime, sessionType, timeOffset),
    FOREIGN KEY(ipDecimal, beginUnixTime) REFERENCES SessionList(ipDecimal, beginUnixTime) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS MarshalZones (
    ipDecimal               INT UNSIGNED,
    beginUnixTime           INT UNSIGNED,
    curUnixTime             INT UNSIGNED,
    curTime                 DATETIME,

    zoneIndex               INT UNSIGNED,
    zoneStart               FLOAT,
    zoneFlag                TINYINT,
    zoneFlagInStr           VARCHAR(16),

    PRIMARY KEY(ipDecimal, curUnixTime, zoneIndex),
    FOREIGN KEY(ipDecimal, beginUnixTime) REFERENCES SessionList(ipDecimal, beginUnixTime) ON DELETE CASCADE
);