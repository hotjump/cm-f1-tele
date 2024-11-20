CREATE PROCEDURE GetTyreWear(
    in ipDec int unsigned,
    in beginUnixTimeJIT int unsigned,
    in curUnixTimeJIT int unsigned
)
SELECT
    curUnixTime AS "time",
    driverName,
    GREATEST(
        tyresDamageRL,
        tyresDamageRR,
        tyresDamageFL,
        tyresDamageFR
    ) AS "磨损"
FROM
    CarDamage
WHERE
    ipDecimal = ipDec
    AND beginUnixTime = beginUnixTimeJIT
    AND curUnixTime <= curUnixTimeJIT
ORDER BY
    curUnixTime;

CREATE PROCEDURE GetFrontLeftWingDamage(
    in ipDec int unsigned,
    in beginUnixTimeJIT int unsigned,
    in curUnixTimeJIT int unsigned
)
SELECT
    curUnixTime AS "time",
    driverName,
    frontLeftWingDamage AS "左前翼"
FROM
    CarDamage
WHERE
    ipDecimal = ipDec
    AND beginUnixTime = beginUnixTimeJIT
    AND curUnixTime <= curUnixTimeJIT
    AND frontLeftWingDamage > 0
ORDER BY
    curUnixTime;

CREATE PROCEDURE GetFrontRightWingDamage(
    in ipDec int unsigned,
    in beginUnixTimeJIT int unsigned,
    in curUnixTimeJIT int unsigned
)
SELECT
    curUnixTime AS "time",
    driverName,
    frontRightWingDamage AS "右前翼"
FROM
    CarDamage
WHERE
    ipDecimal = ipDec
    AND beginUnixTime = beginUnixTimeJIT
    AND curUnixTime <= curUnixTimeJIT
    AND frontRightWingDamage > 0
ORDER BY
    curUnixTime;

CREATE PROCEDURE GetRearWingDamage(
    in ipDec int unsigned,
    in beginUnixTimeJIT int unsigned,
    in curUnixTimeJIT int unsigned
)
SELECT
    curUnixTime AS "time",
    driverName,
    rearWingDamage AS "后翼"
FROM
    CarDamage
WHERE
    ipDecimal = ipDec
    AND beginUnixTime = beginUnixTimeJIT
    AND curUnixTime <= curUnixTimeJIT
    AND rearWingDamage > 0
ORDER BY
    curUnixTime;