use f1_2022;

CREATE TABLE IF NOT EXISTS CarDamage(
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


    PRIMARY KEY(curUnixTime, carIndex),
    KEY(beginUnixTime, carIndex),
    FOREIGN KEY (beginUnixTime) REFERENCES SessionList(beginUnixTime)
);