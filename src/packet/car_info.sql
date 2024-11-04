CREATE DATABASE IF NOT EXISTS f1_2022_tele;
USE f1_2022_tele;

CREATE TABLE IF NOT EXISTS CarDiff (
  ipDecimal           INT UNSIGNED,
  beginUnixTime INT UNSIGNED,
  curUnixTime INT UNSIGNED,
  curTime DATETIME,
  carIndex TINYINT UNSIGNED,
  driverName VARCHAR(48),
  carPosition TINYINT UNSIGNED,
  diffBetweenLeader FLOAT,
  diffBetweenFront FLOAT,
  diffBetweenLeaderJIT FLOAT,
  diffBetweenFrontJIT FLOAT,
  diffBetweenLastlapJIT FLOAT,
  diffBetweenBestlapJIT FLOAT,

  PRIMARY KEY(ipDecimal, beginUnixTime, curUnixTime, carIndex),
  FOREIGN KEY(ipDecimal, beginUnixTime) REFERENCES SessionList(ipDecimal, beginUnixTime) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS CarFocus (
  ipDecimal     INT UNSIGNED,
  beginUnixTime INT UNSIGNED,
  curUnixTime INT UNSIGNED,
  curTime DATETIME,
  carIndex TINYINT UNSIGNED,
  driverName VARCHAR(48),
  carPosition TINYINT UNSIGNED,
  scenes varchar(64),

  PRIMARY KEY(ipDecimal, beginUnixTime, curUnixTime, carIndex),
  FOREIGN KEY(ipDecimal, beginUnixTime) REFERENCES SessionList(ipDecimal, beginUnixTime) ON DELETE CASCADE
);

CREATE PROCEDURE CarFocusData (in ipDecimalJIT int unsigned, in beginUnixTimeJIT int unsigned, in curUnixTimeJIT int unsigned)
SELECT
  CarFocus.carPosition as 'NO.',
  CarFocus.driverName as "车手",
  teamName as "车队",
  CarFocus.curUnixTime as "UNIX",
  scenes,
  currentLapNum as "圈数",
  bestLapTimeInStr as '最快圈',
  if(
    diffBetweenLeader > 0,
    concat("+", FORMAT(diffBetweenLeader, 3), "s"),
    ""
  ) as "差距-头车",
  if(
    diffBetweenFront > 0,
    concat("+", FORMAT(diffBetweenFront, 3), "s"),
    ""
  ) as "差距-前车",
  lastLapTimeInStr as '上一圈',
  sector1TimeInStr as "s1",
  sector2TimeInStr as "s2",
  if (driverStatus = 1, diffBetweenLeaderJIT, NULL) AS "即时差距-头车",
  if (
    driverStatus = 1,
    if(
      drs,
      "DRS",
      if(
        drsActivationDistance,
        CONCAT("DRS: ", drsActivationDistance, "m"),
        if(
          ersDeployMode = 3,
          ersDeployModeInStr,
          currentLapTimeInStr
        )
      )
    ),
    driverStatusChar
  ) as "当前",
  concat(
    (
      CASE
        visualTyreCompound
        when 16 then 'S'
        when 17 then 'M'
        when 18 then 'H'
        when 7 then 'I'
        when 8 then 'W'
        ELSE 'Unkown'
      END
    ),
    "(",
    tyresAgeLaps,
    ")",
    "-",
    GREATEST(
      tyresDamageRL,
      tyresDamageRR,
      tyresDamageFL,
      tyresDamageFR
    ),
    "%"
  ) AS "轮胎-磨损",
  FORMAT(
    if (
      lapDistance >= 0,
      lapDistance / trackLength,
      1 + lapDistance / trackLength
    ),
    2
  ) as "赛道位置"
FROM
  CarFocus
  JOIN LapData USING (ipDecimal, beginUnixTime, curUnixTime, carIndex)
  JOIN CarDiff USING (ipDecimal, beginUnixTime, curUnixTime, carIndex)
  JOIN BestLap Using (ipDecimal, beginUnixTime, curUnixTime, carIndex)
  JOIN CarStatus Using (ipDecimal, beginUnixTime, curUnixTime, carIndex)
  JOIN CarTelemetry Using (ipDecimal, beginUnixTime, curUnixTime, carIndex)
  JOIN CarDamage Using (ipDecimal, beginUnixTime, curUnixTime, carIndex)
  JOIN SessionData Using (ipDecimal, beginUnixTime, curUnixTime)
WHERE
  CarFocus.ipDecimal = ipDecimalJIT AND 
  CarFocus.beginUnixTime = beginUnixTimeJIT AND 
  CarFocus.curUnixTime = curUnixTimeJIT;

delimiter // 
CREATE PROCEDURE CarFocusDataJIT(in ip varchar(20))
BEGIN
DECLARE ipDec int unsigned default 0;
DECLARE beginUnixTimeJIT int unsigned default 0;
DECLARE curUnixTimeJIT int unsigned default 0;

SELECT INET_ATON(ip) INTO ipDec;
select beginUnixTime, curUnixTime INTO beginUnixTimeJIT, curUnixTimeJIT from SessionList 
WHERE ipDecimal = ipDec ORDER BY beginUnixTime desc limit 1;
CALL CarFocusData(ipDec, beginUnixTimeJIT, curUnixTimeJIT);
END //
delimiter ;

CREATE PROCEDURE GetIntervalBetweenFront(in ipDec int unsigned, in beginUnixTimeJIT int unsigned, in curUnixtimeJIT int unsigned)
SELECT CONCAT(CarDiff.carPosition,"-",CarDiff.driverName) as name, diffBetweenFront as diff FROM CarDiff JOIN LapData USING(ipDecimal, beginUnixTime, curUnixTime, carIndex)
WHERE ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND curUnixTime=curUnixtimeJIT Order by CarDiff.carPosition;

CREATE PROCEDURE GetIntervalBetweenLeader(in ipDec int unsigned, in beginUnixTimeJIT int unsigned, in curUnixtimeJIT int unsigned)
SELECT CONCAT(CarDiff.carPosition,"-",CarDiff.driverName) as name, if(driverStatus=1, diffBetweenLeaderJIT, 0) as diff FROM CarDiff JOIN LapData USING(ipDecimal, beginUnixTime, curUnixTime, carIndex)
WHERE ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND curUnixTime=curUnixtimeJIT Order by CarDiff.carPosition;

