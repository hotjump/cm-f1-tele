USE f1_2022_tele;

CREATE TABLE IF NOT EXISTS FinalClassification(
	ipDecimal           INT UNSIGNED,
    beginUnixTime       INT UNSIGNED,
    curUnixTime         INT UNSIGNED,
    curTime             DATETIME,

    carIndex            TINYINT UNSIGNED,
    driverName          VARCHAR(48),
	
	teamName        VARCHAR(16), 
	position        TINYINT UNSIGNED,
	numLaps         TINYINT UNSIGNED,
	gridPosition    TINYINT UNSIGNED,
	points          TINYINT UNSIGNED,
	numPitStops     TINYINT UNSIGNED,
	resultStatus    TINYINT UNSIGNED,
	resultStatusStr VARCHAR(16),

	bestLapTime         INT UNSIGNED,
	bestLapTimeStr      VARCHAR(16),
	totalRaceTime       DOUBLE,
	totalRaceTimeStr    VARCHAR(16),
	penaltiesTime       TINYINT UNSIGNED,
	numPenalties        TINYINT UNSIGNED,
	numTyreStints       TINYINT UNSIGNED,

	tyreStintsVisual    VARCHAR(256),
	overallResult       VARCHAR(32),

    PRIMARY KEY(ipDecimal, beginUnixTime, carIndex),
    FOREIGN KEY(ipDecimal, beginUnixTime) REFERENCES SessionList(ipDecimal, beginUnixTime) ON DELETE CASCADE
);

delimiter // 
CREATE PROCEDURE FinalClassification (in ipDecimalJIT int unsigned, in beginUnixTimeJIT int unsigned, in curUnixTimeJIT int unsigned) 
BEGIN
DECLARE is_race int unsigned default 0;
select
  (
    sessionType >= 10
    AND sessionType <= 12
  ) AS "is_race" INTO is_race
from
  SessionData
WHERE
  ipDecimal = ipDecimalJIT AND beginUnixTime = beginUnixTimeJIT AND curUnixTime = curUnixTimeJIT;
if is_race = 0 
	THEN CALL FinalClassificationQualifying(ipDecimalJIT, beginUnixTimeJIT, curUnixTimeJIT);
ELSEIF is_race = 1 
	THEN CALL FinalClassificationRace(ipDecimalJIT, beginUnixTimeJIT, curUnixTimeJIT);
ELSE
	SELECT "ERROR";
END IF;
END //
delimiter ;

CREATE PROCEDURE FinalClassificationRace (in ipDecimalJIT int unsigned, in beginUnixTimeJIT int unsigned, in curUnixTimeJIT int unsigned)
SELECT
  position as 'NO.',
  if(
    position >= gridPosition,
    concat("+", position - gridPosition),
    cast(position as signed) - cast(gridPosition as signed)
  ) as '位置变化',
  driverName as "车手",
  teamName as "车队",
  bestLapTimeStr as "最速圈",
  numLaps as "圈数",
  numPitStops as "停站",
  overallResult AS "结果",
  points AS "积分"
from
  FinalClassification
WHERE
  ipDecimal = ipDecimalJIT AND beginUnixTime = beginUnixTimeJIT
order by
  position;

CREATE PROCEDURE FinalClassificationQualifying (in ipDecimalJIT int unsigned, in beginUnixTimeJIT int unsigned, in curUnixTimeJIT int unsigned)
SELECT
  position as 'NO.',
  f.driverName as "车手",
  teamName as "车队",
  bestLapSector1TimeInStr as "S1",
  bestLapSector2TimeInStr as "S2",
  bestLapSector3TimeInStr as "S3",
  bestLapTimeStr as "最速圈",
  overallResult AS "结果"
from
  FinalClassification f
  JOIN BestLap Using (ipDecimal, beginUnixTime, curUnixTime, carIndex)
WHERE
  f.ipDecimal = ipDecimalJIT AND f.beginUnixTime = beginUnixTimeJIT
order by
  position;

