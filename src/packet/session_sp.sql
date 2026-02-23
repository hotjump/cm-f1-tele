-- UserNameSelected
DROP PROCEDURE IF EXISTS UserNameSelected;
CREATE PROCEDURE UserNameSelected(IN user VARCHAR(48))
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    SELECT ipOwner
    FROM IpList
    WHERE
        (user = 'admin')
        OR (
            ipOwner IN (SELECT userName FROM PublicUser)
            OR ipOwner = user
        )
    GROUP BY ipOwner
    ORDER BY MAX(updateUnixTime) DESC;
END;

-- UserNameActiveTime
DROP PROCEDURE IF EXISTS UserNameActiveTime;
CREATE PROCEDURE UserNameActiveTime(IN user VARCHAR(48))
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    SELECT
        ipOwner AS `玩家`,
        FROM_UNIXTIME(MAX(updateUnixTime)) AS `最后活跃时间`
    FROM IpList
    WHERE
        (user = 'admin')
        OR (
            ipOwner IN (SELECT userName FROM PublicUser)
            OR ipOwner = user
        )
    GROUP BY ipOwner
    ORDER BY MAX(updateUnixTime) DESC;
END;

-- GetBeginTimeByPlayer
DROP PROCEDURE IF EXISTS GetBeginTimeByPlayer;
CREATE PROCEDURE GetBeginTimeByPlayer(
    IN user VARCHAR(48),
    IN playerID VARCHAR(48),
    IN unixEpochTo INT UNSIGNED
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    SELECT MAX(beginUnixTime)
    FROM SessionList
    LEFT JOIN IpList USING (ipDecimal)
    WHERE
        (
            user = 'admin'
            OR playerID = user
            OR playerID IN (SELECT userName FROM PublicUser)
        )
        AND ipOwner = playerID
        AND beginUnixTime <= unixEpochTo;
END;

-- GetIpDecByPlayer
DROP PROCEDURE IF EXISTS GetIpDecByPlayer;
CREATE PROCEDURE GetIpDecByPlayer(
    IN user VARCHAR(48),
    IN playerID VARCHAR(48),
    IN beginUnixTimeJIT INT UNSIGNED
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    SELECT ipDecimal
    FROM SessionList
    LEFT JOIN IpList USING (ipDecimal)
    WHERE
        (
            user = 'admin'
            OR playerID = user
            OR playerID IN (SELECT userName FROM PublicUser)
        )
        AND ipOwner = playerID
        AND beginUnixTime = beginUnixTimeJIT;
END;

-- GetCurUnixTime
DROP PROCEDURE IF EXISTS GetCurUnixTime;
CREATE PROCEDURE GetCurUnixTime(
    IN ipDec INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED,
    IN unixEpochTo INT UNSIGNED
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    SELECT LEAST(curUnixTime, unixEpochTo)  -- 修正大小写
    FROM SessionList
    WHERE
        ipDecimal = ipDec
        AND beginUnixTime = beginUnixTimeJIT;
END;

-- GetSessionList
DROP PROCEDURE IF EXISTS GetSessionList;
CREATE PROCEDURE GetSessionList(
    IN user VARCHAR(48),
    IN playerID VARCHAR(48)
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    SELECT
        SessionList.beginUnixTime AS `比赛ID`,
        FROM_UNIXTIME(SessionList.beginUnixTime) AS `开始时间`,
        FROM_UNIXTIME(SessionList.curUnixTime) AS `最后活跃时间`,
        CONCAT(
            formulaInStr, '-', trackIdInStr, '-', sessionTypeInStr,
            '-', gameModeInStr, '-', sessionLengthInStr
        ) AS `比赛`,
        ipOwner AS `上传玩家`,
        playerName AS `游戏内名字`,
        ipComeFrom -> '$.data[0].location' AS `运营商信息`,
        ipString AS `上传IP`
    FROM SessionList
    LEFT JOIN IpList USING (ipDecimal)
    LEFT JOIN SessionData USING (ipDecimal, beginUnixTime, curUnixTime)
    WHERE
        (user = 'admin' OR user = playerID OR playerID IN (SELECT userName FROM PublicUser))
        AND ipOwner = playerID
    ORDER BY beginUnixTime DESC;
END;

-- GetSessionTime
DROP PROCEDURE IF EXISTS GetSessionTime;
CREATE PROCEDURE GetSessionTime(
    IN ipDec INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    SELECT
        CONCAT(
            formulaInStr, '-', trackIdInStr, '-', sessionTypeInStr,
            '-', gameModeInStr, '-', sessionLengthInStr
        ) AS `比赛`,
        FROM_UNIXTIME(SessionList.beginUnixTime) AS `开始时间`,
        FROM_UNIXTIME(SessionList.curUnixTime) AS `最后时间`
    FROM SessionList
    LEFT JOIN SessionData USING (ipDecimal, beginUnixTime, curUnixTime)
    WHERE
        ipDecimal = ipDec
        AND beginUnixTime = beginUnixTimeJIT;
END;

-- GetSessionInfo
DROP PROCEDURE IF EXISTS GetSessionInfo;
CREATE PROCEDURE GetSessionInfo(
    IN ipDec INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED,
    IN curUnixTimeJIT INT UNSIGNED
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    SELECT
        CONCAT(
            formulaInStr, '-', trackIdInStr, '-', sessionTypeInStr,
            '-', gameModeInStr, '-', sessionLengthInStr
        ) AS `比赛`,
        FROM_UNIXTIME(beginUnixTimeJIT) AS `现实-开始时间`,
        FROM_UNIXTIME(curUnixTimeJIT) AS `现实-最后时间`,
        timeOfDay * 60 AS `游戏内-当地时间`,
        sessionTimeLeft AS `游戏内-倒计时`,
        totalLaps AS `总圈数`,
        trackLength AS `赛道长度`,
        weatherInStr AS `天气`,
        trackTemperature AS `赛道温度`,
        airTemperature AS `空气温度`,
        safetyCarStatusInStr AS `安全车状态`,
        IF(isSpectating, 'YES', 'NO') AS `OB`
    FROM SessionData
    WHERE
        ipDecimal = ipDec
        AND beginUnixTime = beginUnixTimeJIT
        AND curUnixTime = curUnixTimeJIT;
END;

-- MarshalZones 表（已基本正确，仅补充注释）
CREATE TABLE IF NOT EXISTS MarshalZones (
    ipDecimal INT UNSIGNED,
    beginUnixTime INT UNSIGNED,
    curUnixTime INT UNSIGNED,
    curTime DATETIME,
    zoneIndex INT UNSIGNED,
    zoneStart FLOAT,
    zoneFlag TINYINT,
    zoneFlagInStr VARCHAR(16),
    PRIMARY KEY (ipDecimal, curUnixTime, zoneIndex),
    FOREIGN KEY (ipDecimal, beginUnixTime)
        REFERENCES SessionList(ipDecimal, beginUnixTime)
        ON DELETE CASCADE
);

-- WeatherForecast 表（已基本正确）
CREATE TABLE IF NOT EXISTS WeatherForecast (
    ipDecimal INT UNSIGNED,
    beginUnixTime INT UNSIGNED,
    curUnixTime INT UNSIGNED,
    curTime DATETIME,
    timeOffset INT UNSIGNED,
    sessionType TINYINT UNSIGNED,
    sessionTypeInStr VARCHAR(16),
    weather TINYINT UNSIGNED,
    weatherInStr VARCHAR(16),
    trackTemperature TINYINT,
    trackTemperatureChange TINYINT,
    trackTemperatureChangeInStr VARCHAR(16),
    airTemperature TINYINT,
    airTemperatureChange TINYINT,
    airTemperatureChangeInStr VARCHAR(16),
    rainPercentage TINYINT UNSIGNED,
    PRIMARY KEY (ipDecimal, beginUnixTime, curUnixTime, sessionType, timeOffset),
    FOREIGN KEY (ipDecimal, beginUnixTime)
        REFERENCES SessionList(ipDecimal, beginUnixTime)
        ON DELETE CASCADE
);

-- GetMarshalFlag
DROP PROCEDURE IF EXISTS GetMarshalFlag;
CREATE PROCEDURE GetMarshalFlag(
    IN ipDec INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED,
    IN curUnixTimeJIT INT UNSIGNED
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    SELECT
        curUnixTime AS `time`,
        CONCAT('ZONE-', LPAD(zoneIndex, 2, '0')) AS `zone`,
        zoneFlag
    FROM MarshalZones
    WHERE
        ipDecimal = ipDec
        AND beginUnixTime = beginUnixTimeJIT
        AND curUnixTime <= curUnixTimeJIT
        AND zoneFlag > 0
    GROUP BY curUnixTime, zoneIndex, zoneFlag
    ORDER BY curUnixTime, zoneIndex;
END;