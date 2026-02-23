-- GetDriverName
DROP PROCEDURE IF EXISTS GetDriverName;
CREATE PROCEDURE GetDriverName(
    IN ipDec INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED,
    IN curUnixTimeJIT INT UNSIGNED  -- 修正为大写 T
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    SELECT driverName
    FROM LapData
    WHERE
        ipDecimal = ipDec
        AND beginUnixTime = beginUnixTimeJIT
        AND curUnixTime = curUnixTimeJIT  -- 假设表字段是 curUnixTime
    ORDER BY carPosition;
END;

-- CarPositionChange (修正拼写)
DROP PROCEDURE IF EXISTS CarPositionChange;  -- 原为 CarPostionChange
CREATE PROCEDURE CarPositionChange(
    IN ipDecimalJIT INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED,
    IN curUnixTimeStart INT UNSIGNED,
    IN curUnixTimeEnd INT UNSIGNED,
    IN car INT
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    SELECT
        t1.curUnixTime AS `time`,
        t1.carPosition AS `Pos.`,
        t1.driverName
    FROM (
        SELECT
            ipDecimal,
            beginUnixTime,
            curUnixTime,
            carPosition,
            driverName,
            carIndex
        FROM LapData
        WHERE
            ipDecimal = ipDecimalJIT
            AND beginUnixTime = beginUnixTimeJIT
            AND curUnixTime >= curUnixTimeStart
            AND curUnixTime <= curUnixTimeEnd
            AND carIndex = car
    ) t1
    LEFT JOIN LapData t2
        ON t1.ipDecimal = t2.ipDecimal
        AND t1.beginUnixTime = t2.beginUnixTime
        AND t1.curUnixTime - 1 = t2.curUnixTime
        AND t1.carIndex = t2.carIndex
    WHERE
        t1.carPosition != t2.carPosition
        OR t1.curUnixTime = (
            SELECT MAX(curUnixTime)
            FROM LapData
            WHERE
                ipDecimal = ipDecimalJIT
                AND beginUnixTime = beginUnixTimeJIT
                AND curUnixTime BETWEEN curUnixTimeStart AND curUnixTimeEnd
                AND carIndex = car
        )
        OR t1.curUnixTime = (
            SELECT MIN(curUnixTime)
            FROM LapData
            WHERE
                ipDecimal = ipDecimalJIT
                AND beginUnixTime = beginUnixTimeJIT
                AND curUnixTime BETWEEN curUnixTimeStart AND curUnixTimeEnd
                AND carIndex = car
        );
END;