-- GetRacingLine
DROP PROCEDURE IF EXISTS GetRacingLine;
CREATE PROCEDURE GetRacingLine(
    IN ipDec INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED,
    IN driverNameJIT VARCHAR(48),
    IN lapNum INT UNSIGNED
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    SELECT
        worldPositionX,
        worldPositionZ,
        speed,
        throttle,
        brake,
        currentLapTimeInStr,
        driverName,
        currentLapNum,
        lapDistance
    FROM LapDetails
    WHERE
        ipDecimal = ipDec
        AND beginUnixTime = beginUnixTimeJIT
        AND driverName = driverNameJIT
        AND currentLapNum = lapNum;
END;

-- GetTelemetry
DROP PROCEDURE IF EXISTS GetTelemetry;
CREATE PROCEDURE GetTelemetry(
    IN ipDec INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED,
    IN driverNameJIT VARCHAR(48),
    IN lapNum INT UNSIGNED
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    SELECT
        currentLapTimeInMS,
        lapDistance,
        speed,
        throttle,
        brake,
        engineRPM,
        gear,
        drs,
        driverName,
        currentLapNum
    FROM LapDetails
    WHERE
        ipDecimal = ipDec
        AND beginUnixTime = beginUnixTimeJIT
        AND driverName = driverNameJIT
        AND currentLapNum = lapNum;
END;

-- GetBestLapTelemetry
DROP PROCEDURE IF EXISTS GetBestLapTelemetry;
CREATE PROCEDURE GetBestLapTelemetry(
    IN ipDec INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED,
    IN curUnixTimeJIT INT UNSIGNED,   -- 修正大小写：curUnixtimeJIT → curUnixTimeJIT
    IN driverNameJIT VARCHAR(48)
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    SELECT
        currentLapTimeInMS,
        lapDistance,
        speed,
        throttle,
        brake,
        engineRPM,
        gear,
        drs,
        driverName,
        currentLapNum
    FROM LapDetails
    WHERE
        ipDecimal = ipDec
        AND beginUnixTime = beginUnixTimeJIT
        AND driverName = driverNameJIT
        AND currentLapNum = (
            SELECT bestLapTimeLapNum
            FROM BestLap
            WHERE
                ipDecimal = ipDec
                AND beginUnixTime = beginUnixTimeJIT
                AND curUnixTime = curUnixTimeJIT
                AND driverName = driverNameJIT
        );
END;