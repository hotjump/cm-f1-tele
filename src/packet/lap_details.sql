CREATE DATABASE IF NOT EXISTS f1_2022_tele;
USE f1_2022_tele;

CREATE TABLE IF NOT EXISTS LapDetails (
    ipDecimal           INT UNSIGNED,
    beginUnixTime       INT UNSIGNED,
    curUnixTime         INT UNSIGNED,
    curTime             DATETIME,

    carIndex            TINYINT UNSIGNED,
    driverName          VARCHAR(48),

    currentLapNum       TINYINT UNSIGNED,
    lapDistance         INT UNSIGNED,

    speed               SMALLINT UNSIGNED,                   
    throttle            FLOAT,                  
    steer               FLOAT,                 
    brake               FLOAT,                   
    clutch              TINYINT UNSIGNED,                    
    gear                TINYINT,                       
    engineRPM           SMALLINT UNSIGNED,                
    drs                 TINYINT UNSIGNED,                      

    worldPositionX      FLOAT,
    worldPositionY      FLOAT,
    worldPositionZ      FLOAT,

    currentLapTimeInMS  INT UNSIGNED,
    currentLapTimeInStr VARCHAR(16),

    ersDeployMode       TINYINT UNSIGNED,

    PRIMARY KEY(ipDecimal, beginUnixTime, driverName, currentLapNum, lapDistance),
    FOREIGN KEY(ipDecimal, beginUnixTime) REFERENCES SessionList(ipDecimal, beginUnixTime) ON DELETE CASCADE
);

CREATE PROCEDURE GetRacingLine(in ipDec int unsigned, in beginUnixTimeJIT int unsigned, in driverNameJIT varchar(48),in lapNum int unsigned)
select worldPositionX,worldPositionZ,speed,throttle,brake,currentLapTimeInStr,driverName,currentLapNum,lapDistance from LapDetails 
where ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND driverName=driverNameJIT and currentLapNum=lapNum;

CREATE PROCEDURE GetRacingLine(in ipDec int unsigned, in beginUnixTimeJIT int unsigned, in driverNameJIT varchar(48),in lapNum int unsigned)
select worldPositionX,worldPositionZ,speed,throttle,brake,currentLapTimeInStr,driverName,currentLapNum,lapDistance from LapDetails 
where ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND driverName=driverNameJIT and currentLapNum=lapNum;

CREATE PROCEDURE GetTelemetry(in ipDec int unsigned, in beginUnixTimeJIT int unsigned, in driverNameJIT varchar(48),in lapNum int unsigned)
select currentLapTimeInMS,lapDistance,speed,throttle,brake,engineRPM,gear,drs,driverName,currentLapNum from LapDetails 
where ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND driverName=driverNameJIT and currentLapNum=lapNum;

CREATE PROCEDURE GetBestLapTelemetry(in ipDec int unsigned, in beginUnixTimeJIT int unsigned, in curUnixtimeJIT int unsigned, in driverNameJIT varchar(48))
select currentLapTimeInMS,lapDistance,speed,throttle,brake,engineRPM,gear,drs,driverName,currentLapNum from LapDetails 
where ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND driverName=driverNameJIT and currentLapNum=(select bestLapTimeLapNum from BestLap 
WHERE ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND curUnixTime=curUnixtimeJIT AND driverName=driverNameJIT);