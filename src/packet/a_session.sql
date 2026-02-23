-- PublicUser
CREATE TABLE IF NOT EXISTS PublicUser (
    userName VARCHAR(48) PRIMARY KEY
);

-- IpList
CREATE TABLE IF NOT EXISTS IpList (
    ipDecimal BIGINT PRIMARY KEY,
    updateUnixTime BIGINT,
    updateTime DATETIME,
    ipString VARCHAR(20),
    ipComeFrom JSON,
    ipOwner VARCHAR(48) DEFAULT ''
);

-- SessionList
CREATE TABLE IF NOT EXISTS SessionList (
    ipDecimal BIGINT,
    beginUnixTime BIGINT,
    curUnixTime BIGINT,
    beginTime DATETIME,
    sessionTime FLOAT,
    sessionUID BIGINT,
    frameIdentifier INTEGER,
    packetFormat INTEGER,
    gameMajorVersion INTEGER,
    gameMinorVersion INTEGER,
    packetVersion INTEGER,
    playerCarIndex INTEGER,
    secondaryPlayerCarIndex INTEGER,
    playerName VARCHAR(48),
    secondaryPlayerName VARCHAR(48),  -- fixed typo
    PRIMARY KEY (ipDecimal, beginUnixTime),
    FOREIGN KEY (ipDecimal) REFERENCES IpList(ipDecimal)
);

-- SessionData
CREATE TABLE IF NOT EXISTS SessionData (
    ipDecimal BIGINT,
    beginUnixTime BIGINT,
    curUnixTime BIGINT,
    curTime DATETIME,
    weather SMALLINT,
    weatherInStr VARCHAR(16),
    trackTemperature SMALLINT,
    airTemperature SMALLINT,
    totalLaps SMALLINT,
    trackLength INTEGER,
    sessionType SMALLINT,
    sessionTypeInStr VARCHAR(16),
    trackId SMALLINT,
    trackIdInStr VARCHAR(20),
    formula SMALLINT,
    formulaInStr VARCHAR(16),
    sessionTimeLeft INTEGER,
    sessionDuration INTEGER,
    pitSpeedLimit SMALLINT,
    gamePaused SMALLINT,
    isSpectating SMALLINT,
    spectatorCarIndex SMALLINT,
    sliProNativeSupport SMALLINT,
    numMarshalZones SMALLINT,
    safetyCarStatus SMALLINT,
    safetyCarStatusInStr VARCHAR(16),
    networkGame SMALLINT,
    numWeatherForecastSamples SMALLINT,
    forecastAccuracy SMALLINT,
    aiDifficulty SMALLINT,
    seasonLinkIdentifier BIGINT,
    weekendLinkIdentifier BIGINT,
    sessionLinkIdentifier BIGINT,
    pitStopWindowIdealLap SMALLINT,
    pitStopWindowLatestLap SMALLINT,
    pitStopRejoinPosition SMALLINT,
    steeringAssist SMALLINT,
    brakingAssist SMALLINT,
    gearboxAssist SMALLINT,
    pitAssist SMALLINT,
    pitReleaseAssist SMALLINT,
    ERSAssist SMALLINT,
    DRSAssist SMALLINT,
    dynamicRacingLine SMALLINT,
    dynamicRacingLineType SMALLINT,
    gameMode SMALLINT,
    gameModeInStr VARCHAR(16),
    ruleSet SMALLINT,
    ruleSetInStr VARCHAR(24),
    timeOfDay BIGINT,
    sessionLength SMALLINT,
    sessionLengthInStr VARCHAR(16),
    PRIMARY KEY (ipDecimal, beginUnixTime, curUnixTime),
    FOREIGN KEY (ipDecimal, beginUnixTime) REFERENCES SessionList(ipDecimal, beginUnixTime)
);