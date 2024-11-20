CREATE PROCEDURE GetDriverName(
    in ipDec int unsigned,
    in beginUnixTimeJIT int unsigned,
    in curUnixtimeJIT int unsigned
)
SELECT
    driverName
FROM
    LapData
WHERE
    ipDecimal = ipDec
    AND beginUnixTime = beginUnixTimeJIT
    AND curUnixtime = curUnixtimeJIT
ORDER BY
    carPosition;

CREATE PROCEDURE CarPostionChange (
    in ipDecimalJIT int unsigned,
    in beginUnixTimeJIT int unsigned,
    in curUnixTimeStart int unsigned,
    in curUnixTimeEnd int unsigned,
    in car int
)
SELECT
    t1.curUnixTime AS "time",
    t1.carPosition AS "Pos.",
    t1.driverName
FROM
    (
        SELECT
            ipDecimal,
            beginUnixTime,
            curUnixTime,
            carPosition,
            driverName,
            carIndex
        FROM
            LapData
        WHERE
            ipDecimal = ipDecimalJIT
            AND beginUnixTime = beginUnixTimeJIT
            AND curUnixTime >= curUnixTimeStart
            AND curUnixTime <= curUnixTimeEnd
            AND carIndex = car
    ) t1
    LEFT JOIN LapData t2 ON t1.ipDecimal = t2.ipDecimal
    AND t1.beginUnixTime = t2.beginUnixTime
    AND t1.curUnixTime - 1 = t2.curUnixTime
    AND t1.carIndex = t2.carIndex
WHERE
    t1.carPosition != t2.carPosition
    OR t1.curUnixTime = (
        SELECT
            max(curUnixTime)
        FROM
            LapData
        WHERE
            ipDecimal = ipDecimalJIT
            AND beginUnixTime = beginUnixTimeJIT
            AND curUnixTime >= curUnixTimeStart
            AND curUnixTime <= curUnixTimeEnd
            AND carIndex = car
    )
    OR t1.curUnixTime = (
        SELECT
            min(curUnixTime)
        FROM
            LapData
        WHERE
            ipDecimal = ipDecimalJIT
            AND beginUnixTime = beginUnixTimeJIT
            AND curUnixTime >= curUnixTimeStart
            AND curUnixTime <= curUnixTimeEnd
            AND carIndex = car
    );