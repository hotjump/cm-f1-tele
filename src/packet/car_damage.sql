CREATE DATABASE IF NOT EXISTS f1_2022_tele;
USE f1_2022_tele;

CREATE TABLE IF NOT EXISTS CarDamage(
    ipDecimal           INT UNSIGNED,
    beginUnixTime       INT UNSIGNED,
    curUnixTime         INT UNSIGNED,
    curTime             DATETIME,

    carIndex            TINYINT UNSIGNED,
    driverName          VARCHAR(48),

    tyresWearRL         FLOAT,
    tyresWearRR         FLOAT,
    tyresWearFL         FLOAT,
    tyresWearFR         FLOAT,
    tyresDamageRL       TINYINT UNSIGNED,
    tyresDamageRR       TINYINT UNSIGNED,
    tyresDamageFL       TINYINT UNSIGNED,
    tyresDamageFR       TINYINT UNSIGNED,
    brakesDamageRL      TINYINT UNSIGNED,
    brakesDamageRR      TINYINT UNSIGNED,
    brakesDamageFL      TINYINT UNSIGNED,
    brakesDamageFR      TINYINT UNSIGNED,
    frontLeftWingDamage TINYINT UNSIGNED,
    frontRightWingDamage TINYINT UNSIGNED,
    rearWingDamage      TINYINT UNSIGNED,
    floorDamage         TINYINT UNSIGNED,
    diffuserDamage      TINYINT UNSIGNED,
    sidepodDamage       TINYINT UNSIGNED,
    drsFault            TINYINT UNSIGNED,
    ersFault            TINYINT UNSIGNED,
    gearBoxDamage       TINYINT UNSIGNED,
    engineDamage        TINYINT UNSIGNED,
    engineMGUHWear      TINYINT UNSIGNED, 
    engineESWear        TINYINT UNSIGNED,
    engineCEWear        TINYINT UNSIGNED,
    engineICEWear       TINYINT UNSIGNED,
    engineMGUKWear      TINYINT UNSIGNED,
    engineTCWear        TINYINT UNSIGNED,
    engineBlown         TINYINT UNSIGNED,
    engineSeized        TINYINT UNSIGNED,

    PRIMARY KEY(ipDecimal, beginUnixTime, curUnixTime, carIndex),
    FOREIGN KEY(ipDecimal, beginUnixTime) REFERENCES SessionList(ipDecimal, beginUnixTime) ON DELETE CASCADE
);

CREATE PROCEDURE GetTyreWear(in ipDec int unsigned, in beginUnixTimeJIT int unsigned, in curUnixTimeJIT int unsigned)
SELECT
curUnixTime AS "time", 
driverName,
GREATEST(tyresDamageRL, tyresDamageRR, tyresDamageFL, tyresDamageFR) AS "磨损"
FROM CarDamage
WHERE
   ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND curUnixTime<=curUnixTimeJIT
ORDER BY curUnixTime;

CREATE PROCEDURE GetFrontLeftWingDamage(in ipDec int unsigned, in beginUnixTimeJIT int unsigned, in curUnixTimeJIT int unsigned)
SELECT
curUnixTime AS "time", 
driverName,
frontLeftWingDamage  AS "左前翼"
FROM CarDamage 
WHERE
ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND curUnixTime<=curUnixTimeJIT AND frontLeftWingDamage > 0
ORDER BY curUnixTime;

CREATE PROCEDURE GetFrontRightWingDamage(in ipDec int unsigned, in beginUnixTimeJIT int unsigned, in curUnixTimeJIT int unsigned)
SELECT
curUnixTime AS "time", 
driverName,
frontRightWingDamage AS "右前翼"
FROM CarDamage 
WHERE
ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND curUnixTime<=curUnixTimeJIT AND frontRightWingDamage > 0
ORDER BY curUnixTime;

CREATE PROCEDURE GetRearWingDamage(in ipDec int unsigned, in beginUnixTimeJIT int unsigned, in curUnixTimeJIT int unsigned)
SELECT
curUnixTime AS "time", 
driverName,
rearWingDamage  AS "后翼"
FROM CarDamage 
WHERE
ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND curUnixTime<=curUnixTimeJIT AND rearWingDamage > 0
ORDER BY curUnixTime;
