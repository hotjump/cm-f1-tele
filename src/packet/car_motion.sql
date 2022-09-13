use f1_2022;

CREATE TABLE IF NOT EXISTS CarMotion (
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

    PRIMARY KEY(curUnixTime, carIndex),
    KEY(beginUnixTime, carIndex),
    FOREIGN KEY (beginUnixTime) REFERENCES SessionList(beginUnixTime)
);