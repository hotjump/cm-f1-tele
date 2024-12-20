CREATE TABLE IF NOT EXISTS CarDamage(
    ipDecimal INT UNSIGNED,
    beginUnixTime INT UNSIGNED,
    curUnixTime INT UNSIGNED,
    curTime DATETIME,
    carIndex TINYINT UNSIGNED,
    driverName VARCHAR(48),
    tyresWearRL FLOAT,
    tyresWearRR FLOAT,
    tyresWearFL FLOAT,
    tyresWearFR FLOAT,
    tyresDamageRL TINYINT UNSIGNED,
    tyresDamageRR TINYINT UNSIGNED,
    tyresDamageFL TINYINT UNSIGNED,
    tyresDamageFR TINYINT UNSIGNED,
    brakesDamageRL TINYINT UNSIGNED,
    brakesDamageRR TINYINT UNSIGNED,
    brakesDamageFL TINYINT UNSIGNED,
    brakesDamageFR TINYINT UNSIGNED,
    frontLeftWingDamage TINYINT UNSIGNED,
    frontRightWingDamage TINYINT UNSIGNED,
    rearWingDamage TINYINT UNSIGNED,
    floorDamage TINYINT UNSIGNED,
    diffuserDamage TINYINT UNSIGNED,
    sidepodDamage TINYINT UNSIGNED,
    drsFault TINYINT UNSIGNED,
    ersFault TINYINT UNSIGNED,
    gearBoxDamage TINYINT UNSIGNED,
    engineDamage TINYINT UNSIGNED,
    engineMGUHWear TINYINT UNSIGNED,
    engineESWear TINYINT UNSIGNED,
    engineCEWear TINYINT UNSIGNED,
    engineICEWear TINYINT UNSIGNED,
    engineMGUKWear TINYINT UNSIGNED,
    engineTCWear TINYINT UNSIGNED,
    engineBlown TINYINT UNSIGNED,
    engineSeized TINYINT UNSIGNED,
    PRIMARY KEY(ipDecimal, beginUnixTime, curUnixTime, carIndex),
    FOREIGN KEY(ipDecimal, beginUnixTime) REFERENCES SessionList(ipDecimal, beginUnixTime) ON DELETE CASCADE
);