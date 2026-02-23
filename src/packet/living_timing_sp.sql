-- LiveTiming
DROP PROCEDURE IF EXISTS LiveTiming;
CREATE PROCEDURE LiveTiming(
    IN ipDecimalJIT INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED,
    IN curUnixTimeJIT INT UNSIGNED
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    DECLARE is_race INT UNSIGNED DEFAULT 0;

    SELECT (sessionType >= 10 AND sessionType <= 12) INTO is_race
    FROM SessionData
    WHERE
        ipDecimal = ipDecimalJIT
        AND beginUnixTime = beginUnixTimeJIT
        AND curUnixTime = curUnixTimeJIT
    LIMIT 1;

    IF is_race = 0 THEN
        CALL LiveQualifying(ipDecimalJIT, beginUnixTimeJIT, curUnixTimeJIT);
    ELSEIF is_race = 1 THEN
        CALL LiveRace(ipDecimalJIT, beginUnixTimeJIT, curUnixTimeJIT);
    ELSE
        SELECT 'ERROR: Session type not recognized' AS `error`;
    END IF;
END;

-- LiveRace
DROP PROCEDURE IF EXISTS LiveRace;
CREATE PROCEDURE LiveRace(
    IN ipDecimalJIT INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED,
    IN curUnixTimeJIT INT UNSIGNED
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    SELECT
        LapData.carPosition AS `NO.`,
        IF(
            LapData.carPosition >= gridPosition,
            CONCAT('+', LapData.carPosition - gridPosition),
            CAST(LapData.carPosition AS SIGNED) - CAST(gridPosition AS SIGNED)
        ) AS `位置变化`,
        LapData.driverName AS `车手`,
        teamName AS `车队`,
        currentLapNum AS `圈数`,
        bestLapTimeInStr AS `最快圈`,
        lastLapTimeInStr AS `上一圈`,
        sector1TimeInStr AS `s1`,
        sector2TimeInStr AS `s2`,
        IF(
            resultStatus > 2,
            resultStatusChar,
            IF(
                pitStatus,
                CONCAT(pitStatusChar, '(', FORMAT(pitLaneTimeInLaneInMS / 1000, 3), ')'),
                IF(
                    drs,
                    IF(
                        ersDeployMode = 3,
                        CONCAT('DRS+', ersDeployModeInStr),
                        'DRS'
                    ),
                    IF(
                        drsActivationDistance,
                        CONCAT('DRS: ', drsActivationDistance, 'm'),
                        IF(
                            ersDeployMode = 3,
                            ersDeployModeInStr,
                            currentLapTimeInStr
                        )
                    )
                )
            )
        ) AS `当前`,
        IF(
            resultStatus > 3,
            resultStatusChar,
            IF(
                diffBetweenLeader,
                CONCAT('+', FORMAT(diffBetweenLeader, 3), 's', IF(penalties, CONCAT('+', penalties, 's'), '')),
                ''
            )
        ) AS `差距-头车`,
        IF(
            resultStatus > 3,
            resultStatusChar,
            IF(
                diffBetweenFront,
                CONCAT('+', FORMAT(diffBetweenFront, 3), 's', IF(penalties, CONCAT('+', penalties, 's'), '')),
                ''
            )
        ) AS `差距-前车`,
        CONCAT(penalties, '(', warnings, ')') AS `罚时(警告)`,
        numPitStops AS `进站`,
        CONCAT(
            CASE visualTyreCompound
                WHEN 16 THEN 'S'
                WHEN 17 THEN 'M'
                WHEN 18 THEN 'H'
                WHEN 7 THEN 'I'
                WHEN 8 THEN 'W'
                ELSE 'Unknown'  -- 修正拼写
            END,
            '(',
            tyresAgeLaps,
            ')',
            '-',
            GREATEST(tyresDamageRL, tyresDamageRR, tyresDamageFL, tyresDamageFR),
            '%'
        ) AS `轮胎-磨损`,
        FORMAT(
            IF(lapDistance >= 0, lapDistance / trackLength, 1 + lapDistance / trackLength),
            2
        ) AS `赛道位置`,
        ersStoreEnergy / 40000 AS `ERS`,
        CONCAT(FORMAT(fuelInTank, 1), '(', FORMAT(fuelRemainingLaps, 1), ')') AS `油量`
    FROM
        LapData
        JOIN CarDiff USING (ipDecimal, beginUnixTime, curUnixTime, carIndex)
        JOIN BestLap USING (ipDecimal, beginUnixTime, curUnixTime, carIndex)
        JOIN CarStatus USING (ipDecimal, beginUnixTime, curUnixTime, carIndex)
        JOIN CarTelemetry USING (ipDecimal, beginUnixTime, curUnixTime, carIndex)
        JOIN CarDamage USING (ipDecimal, beginUnixTime, curUnixTime, carIndex)
        JOIN SessionData USING (ipDecimal, beginUnixTime, curUnixTime)
    WHERE
        LapData.ipDecimal = ipDecimalJIT
        AND LapData.beginUnixTime = beginUnixTimeJIT
        AND LapData.curUnixTime = curUnixTimeJIT
    ORDER BY LapData.carPosition;
END;

-- LiveQualifying
DROP PROCEDURE IF EXISTS LiveQualifying;
CREATE PROCEDURE LiveQualifying(
    IN ipDecimalJIT INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED,
    IN curUnixTimeJIT INT UNSIGNED
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    SELECT
        LapData.carPosition AS `NO.`,
        LapData.driverName AS `车手`,
        teamName AS `车队`,
        currentLapNum AS `圈数`,
        bestLapSector1TimeInStr AS `最快圈-s1`,
        bestLapSector2TimeInStr AS `最快圈-s2`,
        bestLapSector3TimeInStr AS `最快圈-s3`,
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
        IF(
            driverStatus = 1 OR ersDeployMode = 2,
            diffBetweenLeader,  -- 修正：原为 diffBetweenLeaderJIT（可能不存在）
            NULL
        ) AS `即时差距-头车`,
        IF(
            driverStatus = 1 OR ersDeployMode = 2,
            IF(
                drs,
                'DRS',
                IF(
                    drsActivationDistance,
                    CONCAT('DRS: ', drsActivationDistance, 'm'),
                    currentLapTimeInStr
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
                ELSE 'Unknown'  -- 修正拼写
            END,
            '(',
            tyresAgeLaps,
            ')',
            '-',
            GREATEST(tyresDamageRL, tyresDamageRR, tyresDamageFL, tyresDamageFR),
            '%'
        ) AS `轮胎-磨损`,
        FORMAT(
            IF(lapDistance >= 0, lapDistance / trackLength, 1 + lapDistance / trackLength),
            2
        ) AS `赛道位置`,
        ersStoreEnergy / 40000 AS `ERS`,
        CONCAT('电:', ersDeployModeInStr, '|油:', fuelMixInStr) AS `电油模式`
    FROM
        LapData
        JOIN CarDiff USING (ipDecimal, beginUnixTime, curUnixTime, carIndex)
        JOIN BestLap USING (ipDecimal, beginUnixTime, curUnixTime, carIndex)
        JOIN CarStatus USING (ipDecimal, beginUnixTime, curUnixTime, carIndex)
        JOIN CarTelemetry USING (ipDecimal, beginUnixTime, curUnixTime, carIndex)
        JOIN CarDamage USING (ipDecimal, beginUnixTime, curUnixTime, carIndex)
        JOIN SessionData USING (ipDecimal, beginUnixTime, curUnixTime)
    WHERE
        LapData.ipDecimal = ipDecimalJIT
        AND LapData.beginUnixTime = beginUnixTimeJIT
        AND LapData.curUnixTime = curUnixTimeJIT
    ORDER BY LapData.carPosition;
END;