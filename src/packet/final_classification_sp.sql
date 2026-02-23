-- FinalClassification
DROP PROCEDURE IF EXISTS FinalClassification;
CREATE PROCEDURE FinalClassification(
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
        CALL FinalClassificationQualifying(ipDecimalJIT, beginUnixTimeJIT, curUnixTimeJIT);
    ELSEIF is_race = 1 THEN
        CALL FinalClassificationRace(ipDecimalJIT, beginUnixTimeJIT, curUnixTimeJIT);
    ELSE
        SELECT 'ERROR: Unable to determine session type' AS `error`;
    END IF;
END;

-- FinalClassificationRace
DROP PROCEDURE IF EXISTS FinalClassificationRace;
CREATE PROCEDURE FinalClassificationRace(
    IN ipDecimalJIT INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED,
    IN curUnixTimeJIT INT UNSIGNED
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    SELECT
        position AS `NO.`,
        IF(
            position >= gridPosition,
            CONCAT('+', position - gridPosition),
            CAST(position AS SIGNED) - CAST(gridPosition AS SIGNED)
        ) AS `位置变化`,
        driverName AS `车手`,
        teamName AS `车队`,
        bestLapTimeStr AS `最速圈`,
        numLaps AS `圈数`,
        numPitStops AS `停站`,
        overallResult AS `结果`,
        points AS `积分`
    FROM FinalClassification
    WHERE
        ipDecimal = ipDecimalJIT
        AND beginUnixTime = beginUnixTimeJIT
    ORDER BY position;
END;

-- FinalClassificationQualifying
DROP PROCEDURE IF EXISTS FinalClassificationQualifying;
CREATE PROCEDURE FinalClassificationQualifying(
    IN ipDecimalJIT INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED,
    IN curUnixTimeJIT INT UNSIGNED
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    SELECT
        f.position AS `NO.`,
        f.driverName AS `车手`,
        teamName AS `车队`,
        bestLapSector1TimeInStr AS `S1`,
        bestLapSector2TimeInStr AS `S2`,
        bestLapSector3TimeInStr AS `S3`,
        bestLapTimeStr AS `最速圈`,
        overallResult AS `结果`
    FROM FinalClassification f
    JOIN BestLap USING (ipDecimal, beginUnixTime, curUnixTime, carIndex)
    WHERE
        f.ipDecimal = ipDecimalJIT
        AND f.beginUnixTime = beginUnixTimeJIT
    ORDER BY f.position;
END;