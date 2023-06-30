USE f1_2022_tele;

CREATE TABLE IF NOT EXISTS LobbyInfo(
    ipDecimal           INT UNSIGNED,
    curUnixTime         INT UNSIGNED,
    curTime             DATETIME,

    carIndex            TINYINT UNSIGNED,
    driverName          VARCHAR(48),

	aiControlled      TINYINT UNSIGNED,
    teamId            TINYINT UNSIGNED,
	teamName          VARCHAR(16),
    nationality       TINYINT UNSIGNED,
    carNumber         TINYINT UNSIGNED,
	
	readyStatus       TINYINT UNSIGNED,
	readyStatusStr    VARCHAR(16),

	PRIMARY KEY(ipDecimal, curUnixTime, carIndex),
    FOREIGN KEY(ipDecimal) REFERENCES IpList(ipDecimal) ON DELETE CASCADE
);

CREATE PROCEDURE GetLobby(in ipDec int unsigned, in curUnixTimeJIT int unsigned)
select  driverName, teamName, carNumber, readyStatusStr from LobbyInfo WHERE ipDecimal=ipDec AND curUnixTime=curUnixTimeJIT order by teamId;