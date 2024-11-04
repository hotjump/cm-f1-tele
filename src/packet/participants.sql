CREATE DATABASE IF NOT EXISTS f1_2022_tele;
USE f1_2022_tele;

CREATE TABLE IF NOT EXISTS Participants (
    ipDecimal           INT UNSIGNED,
    beginUnixTime       INT UNSIGNED,
    curUnixTime         INT UNSIGNED,
    curTime             DATETIME,

    carIndex            TINYINT UNSIGNED,
    driverName          VARCHAR(48),

    aiControlled    TINYINT UNSIGNED,
	driverId        TINYINT UNSIGNED,
    networkId       TINYINT UNSIGNED,
	teamId          TINYINT UNSIGNED,
    teamName        VARCHAR(48),
    myTeam          TINYINT UNSIGNED,
	raceNumber      TINYINT UNSIGNED,
	nationality     TINYINT UNSIGNED,
    nationalityName VARCHAR(20),
	yourTelemetry   TINYINT UNSIGNED,

    PRIMARY KEY(ipDecimal, beginUnixTime, curUnixTime, carIndex),
    FOREIGN KEY(ipDecimal, beginUnixTime) REFERENCES SessionList(ipDecimal, beginUnixTime) ON DELETE CASCADE
);

CREATE PROCEDURE GetParticipants(in ipDec int unsigned, in beginUnixTimeJIT int unsigned, in curUnixTimeJIT int unsigned)
select  carIndex, driverName, teamName, yourTelemetry,aiControlled,networkId  from Participants 
WHERE ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND curUnixTime=curUnixTimeJIT order by teamId;