delimiter / / CREATE PROCEDURE LiveTiming (
    in ipDecimalJIT int unsigned,
    in beginUnixTimeJIT int unsigned,
    in curUnixTimeJIT int unsigned
) BEGIN DECLARE is_race int unsigned default 0;

select
    (
        sessionType >= 10
        AND sessionType <= 12
    ) AS "is_race" INTO is_race
from
    SessionData
WHERE
    ipDecimal = ipDecimalJIT
    AND beginUnixTime = beginUnixTimeJIT
    AND curUnixTime = curUnixTimeJIT;

if is_race = 0 THEN CALL LiveQualifying(ipDecimalJIT, beginUnixTimeJIT, curUnixTimeJIT);

ELSEIF is_race = 1 THEN CALL LiveRace(ipDecimalJIT, beginUnixTimeJIT, curUnixTimeJIT);

ELSE
SELECT
    "ERROR";

END IF;

END / / delimiter;

CREATE PROCEDURE LiveRace (
    in ipDecimalJIT int unsigned,
    in beginUnixTimeJIT int unsigned,
    in curUnixTimeJIT int unsigned
)
SELECT
    LapData.carPosition as 'NO.',
    if(
        LapData.carPosition >= gridPosition,
        concat("+", LapData.carPosition - gridPosition),
        cast(LapData.carPosition as signed) - cast(gridPosition as signed)
    ) as '位置变化',
    LapData.driverName as "车手",
    teamName as "车队",
    currentLapNum as "圈数",
    bestLapTimeInStr as '最快圈',
    lastLapTimeInStr as '上一圈',
    sector1TimeInStr as "s1",
    sector2TimeInStr as "s2",
    if(
        resultStatus > 2,
        resultStatusChar,
        if(
            pitStatus,
            concat(
                pitStatusChar,
                "(",
                FORMAT(pitLaneTimeInLaneInMS / 1000, 3),
                ")"
            ),
            if(
                drs,
                if (
                    ersDeployMode = 3,
                    Concat("DRS+", ersDeployModeInStr),
                    "DRS"
                ),
                if(
                    drsActivationDistance,
                    CONCAT("DRS: ", drsActivationDistance, "m"),
                    if(
                        ersDeployMode = 3,
                        ersDeployModeInStr,
                        currentLapTimeInStr
                    )
                )
            )
        )
    ) as "当前",
    if(
        resultStatus > 3,
        resultStatusChar,
        if(
            diffBetweenLeader,
            concat(
                "+",
                FORMAT(diffBetweenLeader, 3),
                "s",
                if(penalties, concat("+", penalties, "s"), "")
            ),
            ""
        )
    ) as "差距-头车",
    if(
        resultStatus > 3,
        resultStatusChar,
        if(
            diffBetweenFront,
            concat(
                "+",
                FORMAT(diffBetweenFront, 3),
                "s",
                if(penalties, concat("+", penalties, "s"), "")
            ),
            ""
        )
    ) as "差距-前车",
    CONCAT(penalties, "(", warnings, ")") AS "罚时(警告)",
    numPitStops AS "进站",
    concat(
        (
            CASE
                visualTyreCompound
                when 16 then 'S'
                when 17 then 'M'
                when 18 then 'H'
                when 7 then 'I'
                when 8 then 'W'
                ELSE 'Unkown'
            END
        ),
        "(",
        tyresAgeLaps,
        ")",
        "-",
        GREATEST(
            tyresDamageRL,
            tyresDamageRR,
            tyresDamageFL,
            tyresDamageFR
        ),
        "%"
    ) AS "轮胎-磨损",
    FORMAT(
        if (
            lapDistance >= 0,
            lapDistance / trackLength,
            1 + lapDistance / trackLength
        ),
        2
    ) as "赛道位置",
    ersStoreEnergy / 40000 as "ERS",
    CONCAT(
        FORMAT(fuelInTank, 1),
        "(",
        FORMAT(fuelRemainingLaps, 1),
        ")"
    ) as '油量'
FROM
    LapData
    JOIN CarDiff USING(ipDecimal, beginUnixTime, curUnixTime, carIndex)
    JOIN BestLap Using (ipDecimal, beginUnixTime, curUnixTime, carIndex)
    JOIN CarStatus Using (ipDecimal, beginUnixTime, curUnixTime, carIndex)
    JOIN CarTelemetry Using (ipDecimal, beginUnixTime, curUnixTime, carIndex)
    JOIN CarDamage Using (ipDecimal, beginUnixTime, curUnixTime, carIndex)
    JOIN SessionData Using (ipDecimal, beginUnixTime, curUnixTime)
WHERE
    LapData.ipDecimal = ipDecimalJIT
    AND LapData.beginUnixTime = beginUnixTimeJIT
    AND LapData.curUnixTime = curUnixTimeJIT
ORDER BY
    LapData.carPosition;

CREATE PROCEDURE LiveQualifying (
    in ipDecimalJIT int unsigned,
    in beginUnixTimeJIT int unsigned,
    in curUnixTimeJIT int unsigned
)
SELECT
    LapData.carPosition as 'NO.',
    LapData.driverName as "车手",
    teamName as "车队",
    currentLapNum as "圈数",
    bestLapSector1TimeInStr as "最快圈-s1",
    bestLapSector2TimeInStr as "最快圈-s2",
    bestLapSector3TimeInStr as "最快圈-s3",
    bestLapTimeInStr as '最快圈',
    if(
        diffBetweenLeader > 0,
        concat("+", FORMAT(diffBetweenLeader, 3), "s"),
        ""
    ) as "差距-头车",
    if(
        diffBetweenFront > 0,
        concat("+", FORMAT(diffBetweenFront, 3), "s"),
        ""
    ) as "差距-前车",
    lastLapTimeInStr as '上一圈',
    sector1TimeInStr as "s1",
    sector2TimeInStr as "s2",
    if (
        driverStatus = 1
        OR ersDeployMode = 2,
        diffBetweenLeaderJIT,
        NULL
    ) AS "即时差距-头车",
    if (
        driverStatus = 1
        OR ersDeployMode = 2,
        if(
            drs,
            "DRS",
            if(
                drsActivationDistance,
                CONCAT("DRS: ", drsActivationDistance, "m"),
                currentLapTimeInStr
            )
        ),
        driverStatusChar
    ) as "当前",
    concat(
        (
            CASE
                visualTyreCompound
                when 16 then 'S'
                when 17 then 'M'
                when 18 then 'H'
                when 7 then 'I'
                when 8 then 'W'
                ELSE 'Unkown'
            END
        ),
        "(",
        tyresAgeLaps,
        ")",
        "-",
        GREATEST(
            tyresDamageRL,
            tyresDamageRR,
            tyresDamageFL,
            tyresDamageFR
        ),
        "%"
    ) AS "轮胎-磨损",
    FORMAT(
        if (
            lapDistance >= 0,
            lapDistance / trackLength,
            1 + lapDistance / trackLength
        ),
        2
    ) as "赛道位置",
    ersStoreEnergy / 40000 as "ERS",
    concat("电:", ersDeployModeInStr, "|油:", fuelMixInStr) AS "电油模式"
FROM
    LapData
    JOIN CarDiff USING(ipDecimal, beginUnixTime, curUnixTime, carIndex)
    JOIN BestLap Using (ipDecimal, beginUnixTime, curUnixTime, carIndex)
    JOIN CarStatus Using (ipDecimal, beginUnixTime, curUnixTime, carIndex)
    JOIN CarTelemetry Using (ipDecimal, beginUnixTime, curUnixTime, carIndex)
    JOIN CarDamage Using (ipDecimal, beginUnixTime, curUnixTime, carIndex)
    JOIN SessionData Using (ipDecimal, beginUnixTime, curUnixTime)
WHERE
    LapData.ipDecimal = ipDecimalJIT
    AND LapData.beginUnixTime = beginUnixTimeJIT
    AND LapData.curUnixTime = curUnixTimeJIT
ORDER BY
    LapData.carPosition;