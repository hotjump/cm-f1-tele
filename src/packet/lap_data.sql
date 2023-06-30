USE f1_2022_tele;

CREATE TABLE IF NOT EXISTS LapData (
    ipDecimal           INT UNSIGNED,
    beginUnixTime       INT UNSIGNED,
    curUnixTime         INT UNSIGNED,
    curTime             DATETIME,

    carIndex            TINYINT UNSIGNED,
    driverName          VARCHAR(48),
	  teamName	          VARCHAR(16), 

    lastLapTimeInMS     INT UNSIGNED,
	  lastLapTimeInStr    VARCHAR(16),
	  currentLapTimeInMS  INT UNSIGNED,
    currentLapTimeInStr VARCHAR(16),
	  sector1TimeInMS     SMALLINT UNSIGNED,
	  sector1TimeInStr    VARCHAR(16),
	  sector2TimeInMS     SMALLINT UNSIGNED,
	  sector2TimeInStr    VARCHAR(16),

	  lapDistance         FLOAT,
	  totalDistance       FLOAT,
	  safetyCarDelta      FLOAT,
	  carPosition         TINYINT UNSIGNED,
	  currentLapNum       TINYINT UNSIGNED,
	  pitStatus           TINYINT UNSIGNED,
    pitStatusChar       VARCHAR(12),
    numPitStops         TINYINT UNSIGNED,
	  sector              TINYINT UNSIGNED,
	  currentLapInvalid   TINYINT UNSIGNED,
	  penalties           TINYINT UNSIGNED,
    warnings            TINYINT UNSIGNED,
    numUnservedDriveThroughPens TINYINT UNSIGNED,
    numUnservedStopGoPens       TINYINT UNSIGNED,
	  gridPosition        TINYINT UNSIGNED,
	  driverStatus        TINYINT UNSIGNED,
    driverStatusChar    VARCHAR(10),
	  resultStatus        TINYINT UNSIGNED,
    resultStatusChar    VARCHAR(16),

    pitLaneTimerActive  	TINYINT UNSIGNED,
    pitLaneTimeInLaneInMS     SMALLINT UNSIGNED,
    pitStopTimerInMS          SMALLINT UNSIGNED,
    pitStopShouldServePen     TINYINT UNSIGNED,

    PRIMARY KEY(ipDecimal, beginUnixTime, curUnixTime, carIndex),
    FOREIGN KEY(ipDecimal, beginUnixTime) REFERENCES SessionList(ipDecimal, beginUnixTime) ON DELETE CASCADE
);

CREATE PROCEDURE GetDriverName(in ipDec int unsigned, in beginUnixTimeJIT int unsigned, in curUnixtimeJIT int unsigned)
SELECT driverName FROM LapData WHERE  ipDecimal=ipDec AND beginUnixTime=beginUnixTimeJIT AND curUnixtime=curUnixtimeJIT ORDER BY carPosition;

CREATE PROCEDURE CarPostionChange (
    in ipDecimalJIT int unsigned, 
    in beginUnixTimeJIT int unsigned,
    in curUnixTimeStart int unsigned,
    in curUnixTimeEnd int unsigned,
    in car int
  )
SELECT
  t1.curUnixTime AS "time",
  t1.carPosition AS "Pos.",
  t1.driverName
FROM
  (
    SELECT
      ipDecimal,
      beginUnixTime,
      curUnixTime,
      carPosition,
      driverName,
      carIndex
    FROM
      LapData
    WHERE
      ipDecimal = ipDecimalJIT
      AND beginUnixTime = beginUnixTimeJIT
      AND curUnixTime >= curUnixTimeStart
      AND curUnixTime <= curUnixTimeEnd
      AND carIndex = car
  ) t1
  LEFT JOIN LapData t2 ON
  t1.ipDecimal = t2.ipDecimal
  AND t1.beginUnixTime = t2.beginUnixTime
  AND t1.curUnixTime - 1 = t2.curUnixTime
  AND t1.carIndex = t2.carIndex
WHERE
  t1.carPosition != t2.carPosition OR t1.curUnixTime = (
    SELECT
      max(curUnixTime)
    FROM
      LapData
    WHERE
      ipDecimal = ipDecimalJIT
      AND beginUnixTime = beginUnixTimeJIT
      AND curUnixTime >= curUnixTimeStart
      AND curUnixTime <= curUnixTimeEnd
      AND carIndex = car
  ) OR t1.curUnixTime = (
    SELECT
      min(curUnixTime)
    FROM
      LapData
    WHERE
    ipDecimal = ipDecimalJIT
      AND beginUnixTime = beginUnixTimeJIT
      AND curUnixTime >= curUnixTimeStart
      AND curUnixTime <= curUnixTimeEnd
      AND carIndex = car
);