USE f1_2022_tele;

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
