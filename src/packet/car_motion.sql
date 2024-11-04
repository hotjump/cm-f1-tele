CREATE DATABASE IF NOT EXISTS f1_2022_tele;
USE f1_2022_tele;

CREATE TABLE IF NOT EXISTS CarMotion (
    ipDecimal           INT UNSIGNED,
    beginUnixTime       INT UNSIGNED,
    curUnixTime         INT UNSIGNED,
    curTime             DATETIME,

    carIndex            TINYINT UNSIGNED,
    driverName          VARCHAR(48),

    worldPositionX      FLOAT,
    worldPositionY      FLOAT,
    worldPositionZ      FLOAT,
	worldVelocityX      FLOAT,
    worldVelocityY      FLOAT,
    worldVelocityZ      FLOAT,

	worldForwardDirX    SMALLINT,
    worldForwardDirY    SMALLINT,
    worldForwardDirZ    SMALLINT,

	worldRightDirX      SMALLINT,
    worldRightDirY      SMALLINT,
    worldRightDirZ      SMALLINT,
	gForceLateral       FLOAT,
    gForceLongitudinal  FLOAT,
    gForceVertical      FLOAT,
	yaw                 FLOAT,
    pitch               FLOAT,
    roll                FLOAT,

	lat                 FLOAT,
	lon                 FLOAT,

    PRIMARY KEY(ipDecimal, beginUnixTime, curUnixTime, carIndex),
    FOREIGN KEY(ipDecimal, beginUnixTime) REFERENCES SessionList(ipDecimal, beginUnixTime) ON DELETE CASCADE
);