use f1_2022;
SELECT
  *
FROM
  information_schema.Routines
WHERE
  ROUTINE_SCHEMA = "f1_2022" \ G;
## 房间
SELECT
  *
FROM
  LobbyInfo
WHERE
  beginUnixTime = $ current_session_id;
## 车手列表
SELECT
  *
FROM
  Participants
WHERE
  beginUnixTime = $ current_session_id;
## 所有车手
SELECT
  driverName
FROM
  LapData
WHERE
  curUnixtime = LEAST(
    (
      select
        max(beginUnixTime)
      from
        SessionList
      WHERE
        beginUnixTime <= $ __unixEpochTo()
    ),
    $ __unixEpochTo()
  )
ORDER BY
  carPosition;
## 车辆损耗
SELECT
  LapData.carPosition as 'NO.',
  LapData.driverName as "车手",
  teamName as "车队",
  tyresDamageFL AS "轮胎-左前",
  tyresDamageFR AS "轮胎-右前",
  tyresDamageRL AS "轮胎-左后",
  tyresDamageRR AS "轮胎-右后",
  frontLeftWingDamage AS "左前翼",
  frontRightWingDamage AS "右前翼",
  rearWingDamage AS "后翼",
  brakesDamageFL AS "刹车-左前",
  brakesDamageFR AS "刹车-右前",
  brakesDamageRL AS "刹车-左后",
  brakesDamageRR AS "刹车-右后",
  floorDamage AS "底板",
  diffuserDamage AS "扩散器",
  sidepodDamage AS "侧箱",
  drsFault AS "DRS",
  ersFault AS "ERS",
  gearBoxDamage AS "变速箱",
  engineDamage AS "引擎",
  engineMGUHWear,
  engineESWear,
  engineCEWear,
  engineICEWear,
  engineMGUKWear,
  engineTCWear,
  engineBlown,
  engineSeized
FROM
  LapData
  JOIN CarDamage Using (curUnixTime, carIndex)
WHERE
  LapData.curUnixTime = 1662297281
ORDER BY
  LapData.carPosition;
## 排名和diff变动
  DROP PROCEDURE IF EXISTS CarPostionChange;
CREATE PROCEDURE CarPostionChange (
    in curUnixTimeStart int,
    in curUnixTimeEnd int,
    in beginUnixTimeStart int,
    in car int
  )
SELECT
  t1.curUnixTime AS "time",
  t1.carPosition,
  t1.driverName
FROM
  (
    SELECT
      curUnixTime,
      carPosition,
      driverName,
      carIndex
    FROM
      LapData
    WHERE
      curUnixTime >= curUnixTimeStart
      AND curUnixTime <= curUnixTimeEnd
      AND beginUnixTime = beginUnixTimeStart
      AND carIndex = car
  ) t1
  LEFT JOIN LapData t2 ON t1.curUnixTime - 1 = t2.curUnixTime
  AND t1.carIndex = t2.carIndex
WHERE
  t1.carPosition != t2.carPosition || t1.curUnixTime = (
    SELECT
      max(curUnixTime)
    FROM
      LapData
    WHERE
      curUnixTime >= curUnixTimeStart
      AND curUnixTime <= curUnixTimeEnd
      AND beginUnixTime = beginUnixTimeStart
      AND carIndex = car
  ) || t1.curUnixTime = (
    SELECT
      min(curUnixTime)
    FROM
      LapData
    WHERE
      curUnixTime >= curUnixTimeStart
      AND curUnixTime <= curUnixTimeEnd
      AND beginUnixTime = beginUnixTimeStart
      AND carIndex = car
  );
CALL CarPostionChange(1662294336, 1662297244, 1662294333, 13);
CALL CarPostionChange(
    $ __unixEpochFrom(),
    $ __unixEpochTo(),
    $ beginUnixTime,
    13
  );
DROP PROCEDURE IF EXISTS TyresDamageUpdate;
CREATE PROCEDURE TyresDamageUpdate (
    in curUnixTimeStart int,
    in curUnixTimeEnd int,
    in beginUnixTimeStart int,
    in car int
  )
SELECT
  t1.curUnixTime AS "time",
  t1.typres_damage as "磨损最严重轮胎",
  t1.driverName
FROM
  (
    SELECT
      curUnixTime,
      driverName,
      GREATEST(
        tyresDamageRL,
        tyresDamageRR,
        tyresDamageFL,
        tyresDamageFR
      ) AS typres_damage,
      carIndex
    FROM
      CarDamage
    WHERE
      curUnixTime >= curUnixTimeStart
      AND curUnixTime <= curUnixTimeEnd
      AND beginUnixTime = beginUnixTimeStart
      AND carIndex = car
  ) t1
  LEFT JOIN (
    SELECT
      curUnixTime,
      driverName,
      GREATEST(
        tyresDamageRL,
        tyresDamageRR,
        tyresDamageFL,
        tyresDamageFR
      ) AS typres_damage,
      carIndex
    FROM
      CarDamage
    WHERE
      curUnixTime >= curUnixTimeStart
      AND curUnixTime <= curUnixTimeEnd
      AND beginUnixTime = beginUnixTimeStart
      AND carIndex = car
  ) t2 ON t1.curUnixTime - 1 = t2.curUnixTime
  AND t1.carIndex = t2.carIndex
WHERE
  t1.typres_damage != t2.typres_damage || t1.curUnixTime = (
    SELECT
      max(curUnixTime)
    FROM
      CarDamage
    WHERE
      curUnixTime >= curUnixTimeStart
      AND curUnixTime <= curUnixTimeEnd
      AND beginUnixTime = beginUnixTimeStart
      AND carIndex = car
  ) || t1.curUnixTime = (
    SELECT
      min(curUnixTime)
    FROM
      CarDamage
    WHERE
      curUnixTime >= curUnixTimeStart
      AND curUnixTime <= curUnixTimeEnd
      AND beginUnixTime = beginUnixTimeStart
      AND carIndex = car
  );
CALL TyresDamageUpdate(1662294336, 1662297244, 1662294333, 13);
DROP PROCEDURE IF EXISTS RearWingDamage;
CREATE PROCEDURE RearWingDamage (
    in curUnixTimeStart int,
    in curUnixTimeEnd int,
    in beginUnixTimeStart int,
    in car int
  )
SELECT
  MIN(curUnixTime) AS "time",
  rearWingDamage AS "后翼",
  driverName
FROM
  CarDamage
WHERE
  curUnixTime >= curUnixTimeStart
  AND curUnixTime <= curUnixTimeEnd
  AND beginUnixTime = beginUnixTimeStart
  AND carIndex = car
  AND rearWingDamage > 0
UNION ALL
SELECT
  max(curUnixTime) AS "time",
  rearWingDamage AS "后翼",
  driverName
FROM
  CarDamage
WHERE
  curUnixTime >= curUnixTimeStart
  AND curUnixTime <= curUnixTimeEnd
  AND beginUnixTime = beginUnixTimeStart
  AND carIndex = car
  AND rearWingDamage > 0;
CALL RearWingDamage(1662294336, 1662297244, 1662294333, 10);
SELECT
  curUnixTime AS "time",
  driverName,
  rearWingDamage AS "后翼"
FROM
  CarDamage
WHERE
  $ __unixEpochFilter(curUnixTime)
  AND beginUnixTime = $ beginUnixTime
  AND rearWingDamage > 0
ORDER BY
  curUnixTime;
SELECT
  curUnixTime AS "time",
  driverName,
  GREATEST(
    tyresDamageRL,
    tyresDamageRR,
    tyresDamageFL,
    tyresDamageFR
  ) AS "磨损"
FROM
  CarDamage
WHERE
  $ __unixEpochFilter(curUnixTime)
  AND beginUnixTime = $ beginUnixTime
ORDER BY
  curUnixTime;
## 正赛判断
select
  (
    sessionType >= 10
    AND sessionType <= 12
  ) AS "is_race"
from
  SessionData
WHERE
  curUnixTime = 1662294498;
## 圈速
select
  driverName AS "车手",
  lapNum AS "圈数",
  sector1TimeInStr AS "S1",
  sector2TimeInStr AS "S2",
  sector3TimeInStr AS "S3",
  lapTimeInStr AS "时间",
  LPAD(BIN(lapValidBitFlags), 4, 0) AS "Valid",
  CONCAT(
    tyreVisualCompoundInStr,
    "(",
    tyreLapNumUsedInThisStint,
    ")"
  ) AS "轮胎使用"
from
  LapHistoryData
WHERE
  beginUnixTime = $ beginUnixTime
  AND driverName = '$driverName';
## 排位赛大盘 和 正赛大盘
  CALL LiveTiming(1662294498);
CALL LiveTiming(1662207355);
delimiter / / CREATE PROCEDURE LiveTiming (in curUnixTimeJIT int) BEGIN DECLARE is_race int unsigned default 0;
select
  (
    sessionType >= 10
    AND sessionType <= 12
  ) AS "is_race" INTO is_race
from
  SessionData
WHERE
  curUnixTime = curUnixTimeJIT;
if is_race = 0 THEN CALL LiveQualifying(curUnixTimeJIT);
ELSEIF is_race = 1 THEN CALL LiveRace(curUnixTimeJIT);
  ELSE
SELECT
  "ERROR";
END IF;
END / / delimiter;
DROP PROCEDURE IF EXISTS LiveQualifying;
CREATE PROCEDURE LiveQualifying (in curUnixTimeJIT int)
SELECT
  LapData.carPosition as 'NO.',
  LapData.driverName as "车手",
  teamName as "车队",
  currentLapNum as "圈数",
  bestLapSector1TimeInStr as "最快圈-s1",
  bestLapSector1TimeInStr as "最快圈-s2",
  bestLapSector1TimeInStr as "最快圈-s3",
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
  ) as "赛道位置",
  ersStoreEnergy / 40000 as "ERS",
  concat("电:", ersDeployModeInStr, "|油:", fuelMixInStr) AS "电油模式"
FROM
  LapData
  INNER JOIN CarDiff USING(curUnixTime, carIndex)
  JOIN BestLap Using (curUnixTime, carIndex)
  JOIN CarStatus Using (curUnixTime, carIndex)
  JOIN CarTelemetry Using (curUnixTime, carIndex)
  JOIN CarDamage Using (curUnixTime, carIndex)
  JOIN SessionData Using (curUnixTime)
WHERE
  LapData.curUnixTime = curUnixTimeJIT
ORDER BY
  LapData.carPosition;
DROP PROCEDURE IF EXISTS LiveRace;
CREATE PROCEDURE LiveRace (in curUnixTimeJIT int)
SELECT
  LapData.carPosition as 'NO.',
  if(
    LapData.carPosition >= gridPosition,
    concat("+", LapData.carPosition - gridPosition),
    cast(LapData.carPosition as signed) - cast(gridPosition as signed)
  ) as '位置变化',
  LapData.driverName as "车手",
  teamName as "车队",
  currentLapNum as "圈数",
  bestLapTimeInStr as '最快圈',
  lastLapTimeInStr as '上一圈',
  sector1TimeInStr as "s1",
  sector2TimeInStr as "s2",
  if(
    resultStatus > 2,
    resultStatusChar,
    if(
      pitStatus,
      concat(
        pitStatusChar,
        "(",
        FORMAT(pitLaneTimeInLaneInMS / 1000, 3),
        ")"
      ),
      if(
        drs,
        if (
          ersDeployMode = 3,
          Concat("DRS+", ersDeployModeInStr),
          "DRS"
        ),
        if(
          drsActivationDistance,
          CONCAT("DRS: ", drsActivationDistance, "m"),
          if(
            ersDeployMode = 3,
            ersDeployModeInStr,
            currentLapTimeInStr
          )
        )
      )
    )
  ) as "当前",
  if(
    resultStatus > 3,
    resultStatusChar,
    if(
      diffBetweenLeader,
      concat(
        "+",
        FORMAT(diffBetweenLeader, 3),
        "s",
        if(penalties, concat("+", penalties, "s"), "")
      ),
      ""
    )
  ) as "差距-头车",
  if(
    resultStatus > 3,
    resultStatusChar,
    if(
      diffBetweenFront,
      concat(
        "+",
        FORMAT(diffBetweenFront, 3),
        "s",
        if(penalties, concat("+", penalties, "s"), "")
      ),
      ""
    )
  ) as "差距-前车",
  CONCAT(penalties, "(", warnings, ")") AS "罚时(警告)",
  numPitStops AS "进站",
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
  ) as "赛道位置",
  ersStoreEnergy / 40000 as "ERS",
  CONCAT(
    FORMAT(fuelInTank, 1),
    "(",
    FORMAT(fuelRemainingLaps, 1),
    ")"
  ) as '油量'
FROM
  LapData
  INNER JOIN CarDiff USING(curUnixTime, carIndex)
  JOIN BestLap Using (curUnixTime, carIndex)
  JOIN CarStatus Using (curUnixTime, carIndex)
  JOIN CarTelemetry Using (curUnixTime, carIndex)
  JOIN CarDamage Using (curUnixTime, carIndex)
  JOIN SessionData Using (curUnixTime)
WHERE
  LapData.curUnixTime = curUnixTimeJIT
ORDER BY
  LapData.carPosition;
## damage
  CREATE PROCEDURE LiveRace (in curUnixTimeJIT int)
SELECT
  LapData.carPosition as 'NO.',
  LapData.driverName as "车手",
  teamName as "车队",
  tyresWearFL AS "轮胎-左前",
  tyresWearFR AS "轮胎-右前",
  tyresWearRL AS "轮胎-左后",
  tyresWearRR AS "轮胎-右后",
  frontLeftWingDamage AS "左前翼",
  frontRightWingDamage AS "右前翼",
  rearWingDamage AS "后翼"
FROM
  LapData
  INNER JOIN CarDiff USING(curUnixTime, carIndex)
  JOIN BestLap Using (curUnixTime, carIndex)
  JOIN CarStatus Using (curUnixTime, carIndex)
  JOIN CarTelemetry Using (curUnixTime, carIndex)
  JOIN CarDamage Using (curUnixTime, carIndex)
  JOIN SessionData Using (curUnixTime)
WHERE
  LapData.curUnixTime = curUnixTimeJIT
ORDER BY
  LapData.carPosition;
## 结算页
  CALL FinalClassification(1662297281);
CALL FinalClassificationRace(1662297281);
CALL FinalClassificationRace(1662208103);
CALL FinalClassificationQualifying(1662208103);
CALL FastestLap(1662297281);
delimiter / / CREATE PROCEDURE FinalClassification (in curUnixTimeJIT int, in beginUnixTimeJIT int) BEGIN DECLARE is_race int unsigned default 0;
select
  (
    sessionType >= 10
    AND sessionType <= 12
  ) AS "is_race" INTO is_race
from
  SessionData
WHERE
  curUnixTime = curUnixTimeJIT;
if is_race = 0 THEN CALL FinalClassificationQualifying(beginUnixTimeJIT);
ELSEIF is_race = 1 THEN CALL FinalClassificationRace(beginUnixTimeJIT);
  ELSE
SELECT
  "ERROR";
END IF;
END / / delimiter;
CREATE PROCEDURE FinalClassificationRace (in beginUnixTimeJIT int)
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
  beginUnixTime = beginUnixTimeJIT
order by
  position;
DROP PROCEDURE FinalClassificationQualifying;
CREATE PROCEDURE FinalClassificationQualifying (in beginUnixTimeJIT int)
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
  JOIN BestLap Using (curUnixTime, carIndex)
WHERE
  f.beginUnixTime = beginUnixTimeJIT
order by
  position;
CREATE PROCEDURE FastestLap (in curUnixTimeJIT int)
SELECT
  position as 'NO.',
  f.driverName as "车手",
  teamName as "车队",
  bestLapSector1TimeInStr as "S1",
  bestLapSector2TimeInStr as "S2",
  bestLapSector3TimeInStr as "S3",
  bestLapTimeStr as "最速圈"
from
  FinalClassification f
  JOIN BestLap Using (curUnixTime, carIndex)
WHERE
  f.curUnixTime = curUnixTimeJIT
  AND bestLapTime > 0
ORDER BY
  bestLapTime
LIMIT
  1;
DROP PROCEDURE IF EXISTS CarFocusData;
CREATE PROCEDURE CarFocusData (in curUnixTimeJIT int)
SELECT
  CarFocus.carPosition as 'NO.',
  CarFocus.driverName as "车手",
  teamName as "车队",
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
  JOIN LapData USING (curUnixTime, carIndex)
  JOIN CarDiff USING (curUnixTime, carIndex)
  JOIN BestLap Using (curUnixTime, carIndex)
  JOIN CarStatus Using (curUnixTime, carIndex)
  JOIN CarTelemetry Using (curUnixTime, carIndex)
  JOIN CarDamage Using (curUnixTime, carIndex)
  JOIN SessionData Using (curUnixTime)
WHERE
  CarFocus.curUnixTime = curUnixTimeJIT;

CREATE PROCEDURE CarFocusDataJIT()
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
  if(
    resultStatus > 2,
    resultStatusChar,
    if(
      pitStatus,
      concat(
        pitStatusChar,
        "(",
        FORMAT(pitLaneTimeInLaneInMS / 1000, 3),
        ")"
      ),
      if(
        drs,
        if (
          ersDeployMode = 3,
          Concat("DRS+", ersDeployModeInStr),
          "DRS"
        ),
        if(
          drsActivationDistance,
          CONCAT("DRS: ", drsActivationDistance, "m"),
          if(
            ersDeployMode = 3,
            ersDeployModeInStr,
            currentLapTimeInStr
          )
        )
      )
    )
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
  JOIN LapData USING (curUnixTime, carIndex)
  JOIN CarDiff USING (curUnixTime, carIndex)
  JOIN BestLap Using (curUnixTime, carIndex)
  JOIN CarStatus Using (curUnixTime, carIndex)
  JOIN CarTelemetry Using (curUnixTime, carIndex)
  JOIN CarDamage Using (curUnixTime, carIndex)
  JOIN SessionData Using (curUnixTime)
WHERE
  CarFocus.curUnixTime = (select lastUnixTime from SessionList ORDER BY beginUnixTime desc limit 1);

