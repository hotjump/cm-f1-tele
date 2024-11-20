CREATE PROCEDURE GetRacingLine(
    in ipDec int unsigned,
    in beginUnixTimeJIT int unsigned,
    in driverNameJIT varchar(48),
    in lapNum int unsigned
)
select
    worldPositionX,
    worldPositionZ,
    speed,
    throttle,
    brake,
    currentLapTimeInStr,
    driverName,
    currentLapNum,
    lapDistance
from
    LapDetails
where
    ipDecimal = ipDec
    AND beginUnixTime = beginUnixTimeJIT
    AND driverName = driverNameJIT
    and currentLapNum = lapNum;

CREATE PROCEDURE GetRacingLine(
    in ipDec int unsigned,
    in beginUnixTimeJIT int unsigned,
    in driverNameJIT varchar(48),
    in lapNum int unsigned
)
select
    worldPositionX,
    worldPositionZ,
    speed,
    throttle,
    brake,
    currentLapTimeInStr,
    driverName,
    currentLapNum,
    lapDistance
from
    LapDetails
where
    ipDecimal = ipDec
    AND beginUnixTime = beginUnixTimeJIT
    AND driverName = driverNameJIT
    and currentLapNum = lapNum;

CREATE PROCEDURE GetTelemetry(
    in ipDec int unsigned,
    in beginUnixTimeJIT int unsigned,
    in driverNameJIT varchar(48),
    in lapNum int unsigned
)
select
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
from
    LapDetails
where
    ipDecimal = ipDec
    AND beginUnixTime = beginUnixTimeJIT
    AND driverName = driverNameJIT
    and currentLapNum = lapNum;

CREATE PROCEDURE GetBestLapTelemetry(
    in ipDec int unsigned,
    in beginUnixTimeJIT int unsigned,
    in curUnixtimeJIT int unsigned,
    in driverNameJIT varchar(48)
)
select
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
from
    LapDetails
where
    ipDecimal = ipDec
    AND beginUnixTime = beginUnixTimeJIT
    AND driverName = driverNameJIT
    and currentLapNum =(
        select
            bestLapTimeLapNum
        from
            BestLap
        WHERE
            ipDecimal = ipDec
            AND beginUnixTime = beginUnixTimeJIT
            AND curUnixTime = curUnixtimeJIT
            AND driverName = driverNameJIT
    );