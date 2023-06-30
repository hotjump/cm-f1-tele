DROP DATABASE IF EXISTS f1_2022_tele;
CREATE DATABASE IF NOT EXISTS f1_2022_tele;

USE f1_2022_tele;

CREATE TABLE IF NOT EXISTS PublicUser (
    userName                    VARCHAR(48) PRIMARY KEY
);

CREATE TABLE IF NOT EXISTS IpList (
    ipDecimal                   INT UNSIGNED PRIMARY KEY,
    updateUnixTime              INT UNSIGNED,
    updateTime                  DATETIME,
    ipString                    VARCHAR(20),
    ipComeFrom                  JSON,
    ipOwner                     VARCHAR(48) default ''
    KEY `key_ipOwner` (ipOwner)
);

CREATE PROCEDURE UserNameSelected(in user varchar(48))
select ipOwner from IpList WHERE if(user='admin', 1, ipOwner IN (SELECT userName FROM PublicUser) OR ipOwner=user) group by ipOwner order by max(updateUnixTime) desc;

CREATE PROCEDURE UserNameActiveTime(in user varchar(48))
select ipOwner as "玩家", FROM_UNIXTIME(max(updateUnixTime)) as "最后活跃时间" from IpList WHERE if(user='admin', 1, ipOwner IN (SELECT userName FROM PublicUser) OR ipOwner=user) group by ipOwner order by max(updateUnixTime) desc;

CREATE PROCEDURE UserNameActiveTime(in user varchar(48))
select ipOwner as "玩家", FROM_UNIXTIME(max(updateUnixTime)) as "最后活跃时间" from IpList WHERE if(user='admin', 1, ipOwner IN (SELECT userName FROM PublicUser) OR ipOwner=user) group by ipOwner order by max(updateUnixTime) desc;

CREATE PROCEDURE GetBeginTimeByPlayer(in user varchar(48), in playerID varchar(48), in unixEpochTo int unsigned)
SELECT MAX(beginUnixTime) FROM SessionList LEFT JOIN IpList USING(ipDecimal) WHERE if(user='admin' OR IF(playerID IN (SELECT userName FROM PublicUser) OR playerID=user, 1, 0),ipOwner=playerID,0) AND beginUnixTime<=unixEpochTo;

CREATE PROCEDURE GetIpDecByPlayer(in user varchar(48), in playerID varchar(48), in beginUnixTimeJIT int unsigned)
SELECT ipDecimal  FROM SessionList LEFT JOIN IpList USING(ipDecimal) WHERE if(user='admin' OR IF(playerID IN (SELECT userName FROM PublicUser) OR playerID=user, 1, 0),ipOwner=playerID,0) AND beginUnixTime=beginUnixTimeJIT;

CREATE PROCEDURE GetCurUnixTime(in ipDec int unsigned, in beginUnixTimeJIT int unsigned, in unixEpochTo int unsigned)
select LEAST(curUnixTIME,unixEpochTo) from SessionList WHERE ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT;

CREATE PROCEDURE GetSessionList(in user varchar(48), in playerID varchar(48))
SELECT
SessionList.beginUnixTime AS "比赛ID",
FROM_UNIXTIME(SessionList.beginUnixTime) as "开始时间", 
FROM_UNIXTIME(SessionList.curUnixTime) as "最后活跃时间", 
CONCAT(formulaInStr, "-",trackIdInStr, "-", sessionTypeInStr, "-", gameModeInStr,"-", sessionLengthInStr) AS "比赛", ipOwner  AS "上传玩家",
playerName AS "游戏内名字",
ipComeFrom->"$.data[0].location" AS "运营商信息", ipString AS "上传IP"
FROM SessionList LEFT JOIN IpList USING(ipDecimal) LEFT JOIN SessionData USING(ipDecimal,beginUnixTime,curUnixTime) WHERE 
if(user='admin' OR user=playerID OR playerID IN (SELECT userName FROM PublicUser), ipOwner=playerID,0) order by beginUnixTime desc;

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
    playerName                  VARCHAR(48),
    seconddaryPlayerPlayerName  VARCHAR(48),
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

CREATE PROCEDURE GetSessionTime(in ipDec int unsigned, in beginUnixTimeJIT int unsigned)
SELECT
CONCAT(formulaInStr, "-",trackIdInStr, "-", sessionTypeInStr, "-", gameModeInStr,"-", sessionLengthInStr) AS "比赛",
FROM_UNIXTIME(SessionList.beginUnixTime) as "开始时间", 
FROM_UNIXTIME(SessionList.curUnixTime) as "最后时间"
FROM SessionList LEFT JOIN SessionData USING(ipDecimal,beginUnixTime,curUnixTime) WHERE ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT;

CREATE PROCEDURE GetSessionInfo(in ipDec int unsigned, in beginUnixTimeJIT int unsigned, in curUnixTimeJIT int unsigned)
select  CONCAT(formulaInStr, "-",trackIdInStr, "-", sessionTypeInStr, "-", gameModeInStr,"-", sessionLengthInStr)as "比赛", FROM_UNIXTIME(beginUnixTimeJIT) AS "现实-开始时间", FROM_UNIXTIME(curUnixTimeJIT) AS "现实-最后时间", 
timeOfDay*60 as "游戏内-当地时间", sessionTimeLeft as "游戏内-倒计时",totalLaps AS "总圈数", trackLength AS "赛道长度",weatherInStr AS '天气',trackTemperature as "赛道温度",airTemperature "空气温度", safetyCarStatusInStr AS '安全车状态', if(isSpectating, "YES", "NO") AS "OB"
from SessionData 
WHERE ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND curUnixTime=curUnixTimeJIT;

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


CREATE PROCEDURE GetMarshalFlag(in ipDec int unsigned, in beginUnixTimeJIT int unsigned, in curUnixTimeJIT int unsigned)
SELECT
  curUnixTime AS "time",
  CONCAT("ZONE-",LPAD(zoneIndex,2,0)),
  zoneFlag
FROM MarshalZones
WHERE
ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND curUnixTime<=curUnixTimeJIT AND zoneFlag > 0
GROUP BY curUnixTime,zoneIndex,zoneFlag
ORDER BY curUnixTime,zoneIndex;