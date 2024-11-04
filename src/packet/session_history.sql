CREATE DATABASE IF NOT EXISTS f1_2022_tele;
USE f1_2022_tele;

CREATE TABLE IF NOT EXISTS BestLap (
	ipDecimal                   INT UNSIGNED,
    beginUnixTime       		INT UNSIGNED,
    curUnixTime         		INT UNSIGNED,
    curTime             		DATETIME,

    carIndex            		TINYINT UNSIGNED,
    driverName          		VARCHAR(48),

    bestLapTimeLapNum           TINYINT UNSIGNED,
	bestLapSector1TimeInMS      SMALLINT UNSIGNED,
	bestLapSector1TimeInStr     VARCHAR(16),
	bestLapSector2TimeInMS      SMALLINT UNSIGNED,
	bestLapSector2TimeInStr     VARCHAR(16),
	bestLapSector3TimeInMS      SMALLINT UNSIGNED,
	bestLapSector3TimeInStr     VARCHAR(16),
	bestLapTimeInMS             INT UNSIGNED,
	bestLapTimeInStr            VARCHAR(16),

	bestSector1LapNum           TINYINT UNSIGNED,
	bestSector1TimeInMS         SMALLINT UNSIGNED,
	bestSector1TimeInStr    	VARCHAR(16),
	bestSector2LapNum           TINYINT UNSIGNED,
	bestSector2TimeInMS     	SMALLINT UNSIGNED,
	bestSector2TimeInStr     	VARCHAR(16),
	bestSector3LapNum           TINYINT UNSIGNED,
	bestSector3TimeInMS      	SMALLINT UNSIGNED,
	bestSector3TimeInStr	  	VARCHAR(16),
	theoreticalBestLapTimeInMS  INT UNSIGNED,
	theoreticalBestLapTimeInStr VARCHAR(16),

    PRIMARY KEY(ipDecimal, beginUnixTime, curUnixTime, carIndex),
    FOREIGN KEY(ipDecimal, beginUnixTime) REFERENCES SessionList(ipDecimal, beginUnixTime) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS LapHistoryData (
	ipDecimal           INT UNSIGNED,
	beginUnixTime       INT UNSIGNED,
    curUnixTime         INT UNSIGNED,
    curTime             DATETIME,

    carIndex            TINYINT UNSIGNED,
    driverName          VARCHAR(48),

	lapNum                      TINYINT UNSIGNED,

	sector1TimeInMS             SMALLINT UNSIGNED,
	sector1TimeInStr            VARCHAR(16),
	sector2TimeInMS             SMALLINT UNSIGNED,
	sector2TimeInStr            VARCHAR(16),
	sector3TimeInMS             SMALLINT UNSIGNED,
	sector3TimeInStr            VARCHAR(16),
	lapTimeInMS                 INT UNSIGNED,
	lapTimeInStr                VARCHAR(16),
	lapValidBitFlags        	BIT(4),

    tyreActualCompound          INT UNSIGNED,
    tyreActualCompoundInStr     CHAR(16),
    tyreVisualCompound          INT UNSIGNED,
    tyreVisualCompoundInStr     CHAR(16),
    tyreLapNumUsedInThisStint   TINYINT UNSIGNED,

    PRIMARY KEY(ipDecimal, beginUnixTime, carIndex, lapNum),
    FOREIGN KEY(ipDecimal, beginUnixTime) REFERENCES SessionList(ipDecimal, beginUnixTime) ON DELETE CASCADE
);


CREATE PROCEDURE GetLapNum(in ipDec int unsigned, in beginUnixTimeJIT int unsigned, in driverNameJIT varchar(48))
select lapNum From LapHistoryData  WHERE ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND driverName=driverNameJIT AND lapTimeInMS > 0;

CREATE PROCEDURE GetLapTimeByLapNum(in ipDec int unsigned, in beginUnixTimeJIT int unsigned, in driverNameJIT varchar(48), in lapNumJIT int unsigned)
select lapTimeInStr From LapHistoryData  WHERE ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND driverName=driverNameJIT AND lapNum=lapNumJIT;

CREATE PROCEDURE GetBestLapRank(in ipDec int unsigned, in beginUnixTimeJIT int unsigned, in curUnixtimeJIT int unsigned)
select CONCAT((@i:= @i+1),"-", driverName, " ", bestLapTimeInStr,"(",bestLapTimeLapNum,")") AS NAME,
(bestLapTimeInMS - (select min(bestLapTimeInMS) from BestLap 
WHERE ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND curUnixTime=curUnixtimeJIT AND bestLapTimeInMS>0))/1000 AS diff from BestLap,(SELECT @i:=0) as i WHERE 
ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND curUnixTime=curUnixtimeJIT AND bestLapTimeInMS>0 ORDER BY bestLapTimeInMS;

CREATE PROCEDURE GetBestLapNum(in ipDec int unsigned, in beginUnixTimeJIT int unsigned, in curUnixtimeJIT int unsigned, in driverNameJIT varchar(48))
select bestLapTimeLapNum from BestLap 
WHERE ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND curUnixTime=curUnixtimeJIT AND driverName=driverNameJIT;

CREATE PROCEDURE GetBestLapS1Rank(in ipDec int unsigned, in beginUnixTimeJIT int unsigned, in curUnixtimeJIT int unsigned)
select CONCAT((@i:= @i+1),"-", driverName, " ", bestLapSector1TimeInStr,"(",bestLapTimeLapNum,")") AS NAME,(bestLapSector1TimeInMS - (select min(bestLapSector1TimeInMS) from BestLap
WHERE ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND curUnixTime=curUnixtimeJIT  AND bestLapSector1TimeInMS>0))/1000 as diff, 
bestLapSector1TimeInStr from BestLap,(SELECT @i:=0) as i WHERE ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND curUnixTime=curUnixtimeJIT
AND bestLapSector1TimeInMS>0 ORDER BY bestLapSector1TimeInMS;

CREATE PROCEDURE GetBestLapS2Rank(in ipDec int unsigned, in beginUnixTimeJIT int unsigned, in curUnixtimeJIT int unsigned)
select CONCAT((@i:= @i+1),"-", driverName, " ", bestLapSector2TimeInStr,"(",bestLapTimeLapNum,")") AS NAME,(bestLapSector2TimeInMS - (select min(bestLapSector2TimeInMS) from BestLap 
WHERE ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND curUnixTime=curUnixtimeJIT  
AND bestLapSector2TimeInMS>0))/1000 as diff from BestLap,(SELECT @i:=0) as i 
WHERE ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND curUnixTime=curUnixtimeJIT 
AND bestLapSector2TimeInMS>0 ORDER BY bestLapSector2TimeInMS;

CREATE PROCEDURE GetBestLapS3Rank(in ipDec int unsigned, in beginUnixTimeJIT int unsigned, in curUnixtimeJIT int unsigned)
select CONCAT((@i:= @i+1),"-", driverName, " ", bestLapSector3TimeInStr,"(",bestLapTimeLapNum,")") AS NAME,(bestLapSector3TimeInMS - (select min(bestLapSector3TimeInMS) from BestLap 
WHERE ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND curUnixTime=curUnixtimeJIT
AND bestLapSector3TimeInMS>0))/1000 as diff from BestLap,(SELECT @i:=0) as i
WHERE ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND curUnixTime=curUnixtimeJIT
AND bestLapSector3TimeInMS>0 ORDER BY bestLapSector3TimeInMS;

CREATE PROCEDURE GetTheoreticalBestLapRank(in ipDec int unsigned, in beginUnixTimeJIT int unsigned, in curUnixtimeJIT int unsigned)
select CONCAT((@i:= @i+1),"-", driverName, " ", theoreticalBestLapTimeInStr) AS NAME,(theoreticalBestLapTimeInMS - (select min(theoreticalBestLapTimeInMS) from BestLap 
WHERE ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND curUnixTime=curUnixtimeJIT  
AND theoreticalBestLapTimeInMS>0))/1000 as diff from BestLap,(SELECT @i:=0) as i 
WHERE ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND curUnixTime=curUnixtimeJIT
AND theoreticalBestLapTimeInMS>0 ORDER BY theoreticalBestLapTimeInMS;

CREATE PROCEDURE GetTheoreticalBestS1Rank(in ipDec int unsigned, in beginUnixTimeJIT int unsigned, in curUnixtimeJIT int unsigned)
select CONCAT((@i:= @i+1),"-", driverName, " ", bestSector1TimeInStr,"(",bestSector1LapNum,")") AS NAME,(bestSector1TimeInMS - (select min(bestSector1TimeInMS) from BestLap 
WHERE ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND curUnixTime=curUnixtimeJIT 
AND bestSector1TimeInMS>0))/1000 as diff from BestLap,(SELECT @i:=0) as i
WHERE ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND curUnixTime=curUnixtimeJIT AND bestSector1TimeInMS>0 ORDER BY bestSector1TimeInMS;

CREATE PROCEDURE GetTheoreticalBestS2Rank(in ipDec int unsigned, in beginUnixTimeJIT int unsigned, in curUnixtimeJIT int unsigned)
select CONCAT((@i:= @i+1),"-", driverName, " ", bestSector2TimeInStr,"(",bestSector2LapNum,")") AS NAME,(bestSector2TimeInMS - (select min(bestSector2TimeInMS) from BestLap 
WHERE ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND curUnixTime=curUnixtimeJIT
AND bestSector2TimeInMS>0))/1000 as diff from BestLap,(SELECT @i:=0) as i
WHERE ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND curUnixTime=curUnixtimeJIT
AND bestSector2TimeInMS>0 ORDER BY bestSector2TimeInMS;

CREATE PROCEDURE GetTheoreticalBestS3Rank(in ipDec int unsigned, in beginUnixTimeJIT int unsigned, in curUnixtimeJIT int unsigned)
select CONCAT((@i:= @i+1),"-", driverName, " ", bestSector3TimeInStr,"(",bestSector3LapNum,")") AS NAME,(bestSector3TimeInMS - (select min(bestSector3TimeInMS) from BestLap 
WHERE ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND curUnixTime=curUnixtimeJIT AND bestSector3TimeInMS>0))/1000 as diff from BestLap,(SELECT @i:=0) as i 
WHERE ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND curUnixTime=curUnixtimeJIT  AND bestSector3TimeInMS>0 ORDER BY bestSector3TimeInMS;

CREATE PROCEDURE GetLapHistory(in ipDec int unsigned, in beginUnixTimeJIT int unsigned, in driverNameJIT varchar(48))
select driverName,lapNum, FORMAT(lapTimeInMS/1000,3) as LapTime, lapTimeInStr, sector1TimeInStr, sector2TimeInStr, sector3TimeInStr
from LapHistoryData WHERE ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND driverName=driverNameJIT AND lapTimeInMS > 0;

CREATE PROCEDURE GetLapHistoryDetails(in ipDec int unsigned, in beginUnixTimeJIT int unsigned, in driverNameJIT varchar(48))
select driverName AS "车手",lapNum AS "LAP",sector1TimeInStr AS S1,sector2TimeInStr AS S2,sector3TimeInStr AS S3,
lapTimeInStr AS Laptime,LPAD(BIN(lapValidBitFlags),4,0) AS "Valid",
CONCAT(tyreVisualCompoundInStr,"(",tyreLapNumUsedInThisStint,")") AS "轮胎使用" 
from LapHistoryData WHERE ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND driverName=driverNameJIT;
