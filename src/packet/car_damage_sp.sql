-- 1. GetTyreWear
DROP PROCEDURE IF EXISTS GetTyreWear;
CREATE PROCEDURE GetTyreWear(
    IN ipDec INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED,
    IN curUnixTimeJIT INT UNSIGNED
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    SELECT
        curUnixTime AS `time`,
        driverName,
        GREATEST(tyresDamageRL, tyresDamageRR, tyresDamageFL, tyresDamageFR) AS `磨损`
    FROM CarDamage
    WHERE
        ipDecimal = ipDec
        AND beginUnixTime = beginUnixTimeJIT
        AND curUnixTime <= curUnixTimeJIT
    ORDER BY curUnixTime;
END;

-- 2. GetFrontLeftWingDamage
DROP PROCEDURE IF EXISTS GetFrontLeftWingDamage;
CREATE PROCEDURE GetFrontLeftWingDamage(
    IN ipDec INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED,
    IN curUnixTimeJIT INT UNSIGNED
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    SELECT
        curUnixTime AS `time`,
        driverName,
        frontLeftWingDamage AS `左前翼`
    FROM CarDamage
    WHERE
        ipDecimal = ipDec
        AND beginUnixTime = beginUnixTimeJIT
        AND curUnixTime <= curUnixTimeJIT
        AND frontLeftWingDamage > 0
    ORDER BY curUnixTime;
END;

-- 3. GetFrontRightWingDamage
DROP PROCEDURE IF EXISTS GetFrontRightWingDamage;
CREATE PROCEDURE GetFrontRightWingDamage(
    IN ipDec INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED,
    IN curUnixTimeJIT INT UNSIGNED
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    SELECT
        curUnixTime AS `time`,
        driverName,
        frontRightWingDamage AS `右前翼`
    FROM CarDamage
    WHERE
        ipDecimal = ipDec
        AND beginUnixTime = beginUnixTimeJIT
        AND curUnixTime <= curUnixTimeJIT
        AND frontRightWingDamage > 0
    ORDER BY curUnixTime;
END;

-- 4. GetRearWingDamage
DROP PROCEDURE IF EXISTS GetRearWingDamage;
CREATE PROCEDURE GetRearWingDamage(
    IN ipDec INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED,
    IN curUnixTimeJIT INT UNSIGNED
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    SELECT
        curUnixTime AS `time`,
        driverName,
        rearWingDamage AS `后翼`
    FROM CarDamage
    WHERE
        ipDecimal = ipDec
        AND beginUnixTime = beginUnixTimeJIT
        AND curUnixTime <= curUnixTimeJIT
        AND rearWingDamage > 0
    ORDER BY curUnixTime;
END;