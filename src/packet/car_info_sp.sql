-- CarFocusData
DROP PROCEDURE IF EXISTS CarFocusData;
CREATE PROCEDURE CarFocusData(
    IN ipDecimalJIT INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED,
    IN curUnixTimeJIT INT UNSIGNED
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    SELECT
        CarFocus.carPosition AS `NO.`,
        CarFocus.driverName AS `车手`,
        teamName AS `车队`,
        CarFocus.curUnixTime AS `UNIX`,
        scenes,
        currentLapNum AS `圈数`,
        bestLapTimeInStr AS `最快圈`,
        IF(
            diffBetweenLeader > 0,
            CONCAT('+', FORMAT(diffBetweenLeader, 3), 's'),
            ''
        ) AS `差距-头车`,
        IF(
            diffBetweenFront > 0,
            CONCAT('+', FORMAT(diffBetweenFront, 3), 's'),
            ''
        ) AS `差距-前车`,
        lastLapTimeInStr AS `上一圈`,
        sector1TimeInStr AS `s1`,
        sector2TimeInStr AS `s2`,
        IF(driverStatus = 1, diffBetweenLeaderJIT, NULL) AS `即时差距-头车`,
        IF(
            driverStatus = 1,
            IF(
                drs,
                'DRS',
                IF(
                    drsActivationDistance,
                    CONCAT('DRS: ', drsActivationDistance, 'm'),
                    IF(
                        ersDeployMode = 3,
                        ersDeployModeInStr,
                        currentLapTimeInStr
                    )
                )
            ),
            driverStatusChar
        ) AS `当前`,
        CONCAT(
            CASE visualTyreCompound
                WHEN 16 THEN 'S'
                WHEN 17 THEN 'M'
                WHEN 18 THEN 'H'
                WHEN 7 THEN 'I'
                WHEN 8 THEN 'W'
                ELSE 'Unknown'
            END,
            '(',
            tyresAgeLaps,
            ')',
            '-',
            GREATEST(tyresDamageRL, tyresDamageRR, tyresDamageFL, tyresDamageFR),
            '%'
        ) AS `轮胎-磨损`,
        FORMAT(
            IF(
                lapDistance >= 0,
                lapDistance / trackLength,
                1 + lapDistance / trackLength
            ),
            2
        ) AS `赛道位置`
    FROM
        CarFocus
        JOIN LapData USING (ipDecimal, beginUnixTime, curUnixTime, carIndex)
        JOIN CarDiff USING (ipDecimal, beginUnixTime, curUnixTime, carIndex)
        JOIN BestLap USING (ipDecimal, beginUnixTime, curUnixTime, carIndex)
        JOIN CarStatus USING (ipDecimal, beginUnixTime, curUnixTime, carIndex)
        JOIN CarTelemetry USING (ipDecimal, beginUnixTime, curUnixTime, carIndex)
        JOIN CarDamage USING (ipDecimal, beginUnixTime, curUnixTime, carIndex)
        JOIN SessionData USING (ipDecimal, beginUnixTime, curUnixTime)
    WHERE
        CarFocus.ipDecimal = ipDecimalJIT
        AND CarFocus.beginUnixTime = beginUnixTimeJIT
        AND CarFocus.curUnixTime = curUnixTimeJIT;
END;

-- CarFocusDataJIT
DROP PROCEDURE IF EXISTS CarFocusDataJIT;
CREATE PROCEDURE CarFocusDataJIT(
    IN ip VARCHAR(20)
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    DECLARE ipDec INT UNSIGNED DEFAULT 0;
    DECLARE beginUnixTimeJIT INT UNSIGNED DEFAULT 0;
    DECLARE curUnixTimeJIT INT UNSIGNED DEFAULT 0;

    SELECT INET_ATON(ip) INTO ipDec;

    SELECT beginUnixTime, curUnixTime
    INTO beginUnixTimeJIT, curUnixTimeJIT
    FROM SessionList
    WHERE ipDecimal = ipDec
    ORDER BY beginUnixTime DESC
    LIMIT 1;

    CALL CarFocusData(ipDec, beginUnixTimeJIT, curUnixTimeJIT);
END;

-- GetIntervalBetweenFront
DROP PROCEDURE IF EXISTS GetIntervalBetweenFront;
CREATE PROCEDURE GetIntervalBetweenFront(
    IN ipDec INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED,
    IN curUnixTimeJIT INT UNSIGNED
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    SELECT
        CONCAT(CarDiff.carPosition, '-', CarDiff.driverName) AS name,
        diffBetweenFront AS diff
    FROM
        CarDiff
        JOIN LapData USING (ipDecimal, beginUnixTime, curUnixTime, carIndex)
    WHERE
        ipDecimal = ipDec
        AND beginUnixTime = beginUnixTimeJIT
        AND curUnixTime = curUnixTimeJIT
    ORDER BY CarDiff.carPosition;
END;

-- GetIntervalBetweenLeader
DROP PROCEDURE IF EXISTS GetIntervalBetweenLeader;
CREATE PROCEDURE GetIntervalBetweenLeader(
    IN ipDec INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED,
    IN curUnixTimeJIT INT UNSIGNED
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    SELECT
        CONCAT(CarDiff.carPosition, '-', CarDiff.driverName) AS name,
        IF(driverStatus = 1, diffBetweenLeaderJIT, 0) AS diff
    FROM
        CarDiff
        JOIN LapData USING (ipDecimal, beginUnixTime, curUnixTime, carIndex)
    WHERE
        ipDecimal = ipDec
        AND beginUnixTime = beginUnixTimeJIT
        AND curUnixTime = curUnixTimeJIT
    ORDER BY CarDiff.carPosition;
END;