-- GetLapNum
DROP PROCEDURE IF EXISTS GetLapNum;
CREATE PROCEDURE GetLapNum(
    IN ipDec INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED,
    IN driverNameJIT VARCHAR(48)
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    SELECT lapNum
    FROM LapHistoryData
    WHERE
        ipDecimal = ipDec
        AND beginUnixTime = beginUnixTimeJIT
        AND driverName = driverNameJIT
        AND lapTimeInMS > 0;
END;

-- GetLapTimeByLapNum
DROP PROCEDURE IF EXISTS GetLapTimeByLapNum;
CREATE PROCEDURE GetLapTimeByLapNum(
    IN ipDec INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED,
    IN driverNameJIT VARCHAR(48),
    IN lapNumJIT INT UNSIGNED
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    SELECT lapTimeInStr
    FROM LapHistoryData
    WHERE
        ipDecimal = ipDec
        AND beginUnixTime = beginUnixTimeJIT
        AND driverName = driverNameJIT
        AND lapNum = lapNumJIT;
END;

-- GetBestLapRank (使用 ROW_NUMBER)
DROP PROCEDURE IF EXISTS GetBestLapRank;
CREATE PROCEDURE GetBestLapRank(
    IN ipDec INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED,
    IN curUnixTimeJIT INT UNSIGNED
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    WITH Ranked AS (
        SELECT
            driverName,
            bestLapTimeInStr,
            bestLapTimeLapNum,
            bestLapTimeInMS,
            ROW_NUMBER() OVER (ORDER BY bestLapTimeInMS) AS rn,
            MIN(bestLapTimeInMS) OVER () AS minTime
        FROM BestLap
        WHERE
            ipDecimal = ipDec
            AND beginUnixTime = beginUnixTimeJIT
            AND curUnixTime = curUnixTimeJIT
            AND bestLapTimeInMS > 0
    )
    SELECT
        CONCAT(rn, '-', driverName, ' ', bestLapTimeInStr, '(', bestLapTimeLapNum, ')') AS `NAME`,
        (bestLapTimeInMS - minTime) / 1000 AS diff
    FROM Ranked
    ORDER BY bestLapTimeInMS;
END;

-- GetBestLapNum
DROP PROCEDURE IF EXISTS GetBestLapNum;
CREATE PROCEDURE GetBestLapNum(
    IN ipDec INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED,
    IN curUnixTimeJIT INT UNSIGNED,
    IN driverNameJIT VARCHAR(48)
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    SELECT bestLapTimeLapNum
    FROM BestLap
    WHERE
        ipDecimal = ipDec
        AND beginUnixTime = beginUnixTimeJIT
        AND curUnixTime = curUnixTimeJIT
        AND driverName = driverNameJIT;
END;

-- 通用模板：生成 Sector 排名（S1/S2/S3 / Theoretical）
-- 定义一个辅助视图或直接复用 CTE（此处内联）

-- GetBestLapS1Rank
DROP PROCEDURE IF EXISTS GetBestLapS1Rank;
CREATE PROCEDURE GetBestLapS1Rank(
    IN ipDec INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED,
    IN curUnixTimeJIT INT UNSIGNED
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    WITH Ranked AS (
        SELECT
            driverName,
            bestLapSector1TimeInStr,
            bestLapTimeLapNum,
            bestLapSector1TimeInMS,
            ROW_NUMBER() OVER (ORDER BY bestLapSector1TimeInMS) AS rn,
            MIN(bestLapSector1TimeInMS) OVER () AS minTime
        FROM BestLap
        WHERE
            ipDecimal = ipDec
            AND beginUnixTime = beginUnixTimeJIT
            AND curUnixTime = curUnixTimeJIT
            AND bestLapSector1TimeInMS > 0
    )
    SELECT
        CONCAT(rn, '-', driverName, ' ', bestLapSector1TimeInStr, '(', bestLapTimeLapNum, ')') AS `NAME`,
        (bestLapSector1TimeInMS - minTime) / 1000 AS diff,
        bestLapSector1TimeInStr
    FROM Ranked
    ORDER BY bestLapSector1TimeInMS;
END;

-- GetBestLapS2Rank
DROP PROCEDURE IF EXISTS GetBestLapS2Rank;
CREATE PROCEDURE GetBestLapS2Rank(
    IN ipDec INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED,
    IN curUnixTimeJIT INT UNSIGNED
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    WITH Ranked AS (
        SELECT
            driverName,
            bestLapSector2TimeInStr,
            bestLapTimeLapNum,
            bestLapSector2TimeInMS,
            ROW_NUMBER() OVER (ORDER BY bestLapSector2TimeInMS) AS rn,
            MIN(bestLapSector2TimeInMS) OVER () AS minTime
        FROM BestLap
        WHERE
            ipDecimal = ipDec
            AND beginUnixTime = beginUnixTimeJIT
            AND curUnixTime = curUnixTimeJIT
            AND bestLapSector2TimeInMS > 0
    )
    SELECT
        CONCAT(rn, '-', driverName, ' ', bestLapSector2TimeInStr, '(', bestLapTimeLapNum, ')') AS `NAME`,
        (bestLapSector2TimeInMS - minTime) / 1000 AS diff
    FROM Ranked
    ORDER BY bestLapSector2TimeInMS;
END;

-- GetBestLapS3Rank
DROP PROCEDURE IF EXISTS GetBestLapS3Rank;
CREATE PROCEDURE GetBestLapS3Rank(
    IN ipDec INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED,
    IN curUnixTimeJIT INT UNSIGNED
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    WITH Ranked AS (
        SELECT
            driverName,
            bestLapSector3TimeInStr,
            bestLapTimeLapNum,
            bestLapSector3TimeInMS,
            ROW_NUMBER() OVER (ORDER BY bestLapSector3TimeInMS) AS rn,
            MIN(bestLapSector3TimeInMS) OVER () AS minTime
        FROM BestLap
        WHERE
            ipDecimal = ipDec
            AND beginUnixTime = beginUnixTimeJIT
            AND curUnixTime = curUnixTimeJIT
            AND bestLapSector3TimeInMS > 0
    )
    SELECT
        CONCAT(rn, '-', driverName, ' ', bestLapSector3TimeInStr, '(', bestLapTimeLapNum, ')') AS `NAME`,
        (bestLapSector3TimeInMS - minTime) / 1000 AS diff
    FROM Ranked
    ORDER BY bestLapSector3TimeInMS;
END;

-- GetTheoreticalBestLapRank
DROP PROCEDURE IF EXISTS GetTheoreticalBestLapRank;
CREATE PROCEDURE GetTheoreticalBestLapRank(
    IN ipDec INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED,
    IN curUnixTimeJIT INT UNSIGNED
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    WITH Ranked AS (
        SELECT
            driverName,
            theoreticalBestLapTimeInStr,
            theoreticalBestLapTimeInMS,
            ROW_NUMBER() OVER (ORDER BY theoreticalBestLapTimeInMS) AS rn,
            MIN(theoreticalBestLapTimeInMS) OVER () AS minTime
        FROM BestLap
        WHERE
            ipDecimal = ipDec
            AND beginUnixTime = beginUnixTimeJIT
            AND curUnixTime = curUnixTimeJIT
            AND theoreticalBestLapTimeInMS > 0
    )
    SELECT
        CONCAT(rn, '-', driverName, ' ', theoreticalBestLapTimeInStr) AS `NAME`,
        (theoreticalBestLapTimeInMS - minTime) / 1000 AS diff
    FROM Ranked
    ORDER BY theoreticalBestLapTimeInMS;
END;

-- GetTheoreticalBestS1Rank
DROP PROCEDURE IF EXISTS GetTheoreticalBestS1Rank;
CREATE PROCEDURE GetTheoreticalBestS1Rank(
    IN ipDec INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED,
    IN curUnixTimeJIT INT UNSIGNED
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    WITH Ranked AS (
        SELECT
            driverName,
            bestSector1TimeInStr,
            bestSector1LapNum,
            bestSector1TimeInMS,
            ROW_NUMBER() OVER (ORDER BY bestSector1TimeInMS) AS rn,
            MIN(bestSector1TimeInMS) OVER () AS minTime
        FROM BestLap
        WHERE
            ipDecimal = ipDec
            AND beginUnixTime = beginUnixTimeJIT
            AND curUnixTime = curUnixTimeJIT
            AND bestSector1TimeInMS > 0
    )
    SELECT
        CONCAT(rn, '-', driverName, ' ', bestSector1TimeInStr, '(', bestSector1LapNum, ')') AS `NAME`,
        (bestSector1TimeInMS - minTime) / 1000 AS diff
    FROM Ranked
    ORDER BY bestSector1TimeInMS;
END;

-- GetTheoreticalBestS2Rank
DROP PROCEDURE IF EXISTS GetTheoreticalBestS2Rank;
CREATE PROCEDURE GetTheoreticalBestS2Rank(
    IN ipDec INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED,
    IN curUnixTimeJIT INT UNSIGNED
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    WITH Ranked AS (
        SELECT
            driverName,
            bestSector2TimeInStr,
            bestSector2LapNum,
            bestSector2TimeInMS,
            ROW_NUMBER() OVER (ORDER BY bestSector2TimeInMS) AS rn,
            MIN(bestSector2TimeInMS) OVER () AS minTime
        FROM BestLap
        WHERE
            ipDecimal = ipDec
            AND beginUnixTime = beginUnixTimeJIT
            AND curUnixTime = curUnixTimeJIT
            AND bestSector2TimeInMS > 0
    )
    SELECT
        CONCAT(rn, '-', driverName, ' ', bestSector2TimeInStr, '(', bestSector2LapNum, ')') AS `NAME`,
        (bestSector2TimeInMS - minTime) / 1000 AS diff
    FROM Ranked
    ORDER BY bestSector2TimeInMS;
END;

-- GetTheoreticalBestS3Rank
DROP PROCEDURE IF EXISTS GetTheoreticalBestS3Rank;
CREATE PROCEDURE GetTheoreticalBestS3Rank(
    IN ipDec INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED,
    IN curUnixTimeJIT INT UNSIGNED
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    WITH Ranked AS (
        SELECT
            driverName,
            bestSector3TimeInStr,
            bestSector3LapNum,
            bestSector3TimeInMS,
            ROW_NUMBER() OVER (ORDER BY bestSector3TimeInMS) AS rn,
            MIN(bestSector3TimeInMS) OVER () AS minTime
        FROM BestLap
        WHERE
            ipDecimal = ipDec
            AND beginUnixTime = beginUnixTimeJIT
            AND curUnixTime = curUnixTimeJIT
            AND bestSector3TimeInMS > 0
    )
    SELECT
        CONCAT(rn, '-', driverName, ' ', bestSector3TimeInStr, '(', bestSector3LapNum, ')') AS `NAME`,
        (bestSector3TimeInMS - minTime) / 1000 AS diff
    FROM Ranked
    ORDER BY bestSector3TimeInMS;
END;

-- GetLapHistory
DROP PROCEDURE IF EXISTS GetLapHistory;
CREATE PROCEDURE GetLapHistory(
    IN ipDec INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED,
    IN driverNameJIT VARCHAR(48)
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    SELECT
        driverName,
        lapNum,
        FORMAT(lapTimeInMS / 1000, 3) AS LapTime,
        lapTimeInStr,
        sector1TimeInStr,
        sector2TimeInStr,
        sector3TimeInStr
    FROM LapHistoryData
    WHERE
        ipDecimal = ipDec
        AND beginUnixTime = beginUnixTimeJIT
        AND driverName = driverNameJIT
        AND lapTimeInMS > 0;
END;

-- GetLapHistoryDetails
DROP PROCEDURE IF EXISTS GetLapHistoryDetails;
CREATE PROCEDURE GetLapHistoryDetails(
    IN ipDec INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED,
    IN driverNameJIT VARCHAR(48)
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    SELECT
        driverName AS `车手`,
        lapNum AS `LAP`,
        sector1TimeInStr AS S1,
        sector2TimeInStr AS S2,
        sector3TimeInStr AS S3,
        lapTimeInStr AS Laptime,
        LPAD(BIN(lapValidBitFlags), 4, '0') AS `Valid`,
        CONCAT(
            tyreVisualCompoundInStr,
            '(',
            tyreLapNumUsedInThisStint,
            ')'
        ) AS `轮胎使用`
    FROM LapHistoryData
    WHERE
        ipDecimal = ipDec
        AND beginUnixTime = beginUnixTimeJIT
        AND driverName = driverNameJIT;
END;