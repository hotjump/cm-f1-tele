USE f1_2022_tele;

CREATE TABLE IF NOT EXISTS EventUpdate (
	ipDecimal           INT UNSIGNED,
    beginUnixTime       INT UNSIGNED,
    curUnixTime         INT UNSIGNED,
    curTime             DATETIME,

	eventIndex 			INT UNSIGNED,

	code                CHAR(4),
	driverName          VARCHAR(48),
	description         VARCHAR(64),
    
	PRIMARY KEY(ipDecimal, beginUnixTime, curUnixTime, eventIndex),
    FOREIGN KEY(ipDecimal, beginUnixTime) REFERENCES SessionList(ipDecimal, beginUnixTime) ON DELETE CASCADE
);


CREATE PROCEDURE GetTrafficLight(in ipDec int unsigned, in beginUnixTimeJIT int unsigned, in curUnixTimeJIT int unsigned)
select  description, if (code='DRSE', 80, if(code ='DRSD', 20, if((select count(*) from EventUpdate
WHERE ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND curUnixTime<=curUnixTimeJIT AND code = "LGOT")= 1, NULL,20))) AS "颜色" from EventUpdate 
WHERE ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND curUnixTime<=curUnixTimeJIT AND code IN ("STLG",  "DRSE" ,"DRSD");

CREATE PROCEDURE GetSpeedTrap(in ipDec int unsigned, in beginUnixTimeJIT int unsigned, in curUnixTimeJIT int unsigned)
SELECT CONCAT((@i:= @i+1),"-", driverName) AS NAME, Speed FROM (select driverName, MAX(SUBSTRING(description, -9, 5) + 0)  AS "Speed" 
from EventUpdate 
WHERE ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT  AND curUnixTime<=curUnixTimeJIT
AND code = "SPTP" GROUP BY driverName Order by Speed desc) AS t, (SELECT @i:=0) as i;

CREATE TABLE IF NOT EXISTS PenaltyUpdate (
	ipDecimal           INT UNSIGNED,
    beginUnixTime       INT UNSIGNED,
    curUnixTime         INT UNSIGNED,
    curTime             DATETIME,

	penaltyIndex 		INT UNSIGNED,

	code                CHAR(4),
	driverName          VARCHAR(48),
	otherDriverName     VARCHAR(48),
	penaltyDescription  VARCHAR(64),
	infringementDescription VARCHAR(64),
	timeGained          INT UNSIGNED, 
	lapNum              INT UNSIGNED,
	placesGained        INT UNSIGNED,
    
	PRIMARY KEY(ipDecimal, beginUnixTime, curUnixTime, penaltyIndex),
    FOREIGN KEY(ipDecimal, beginUnixTime) REFERENCES SessionList(ipDecimal, beginUnixTime) ON DELETE CASCADE
);