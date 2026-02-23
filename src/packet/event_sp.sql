-- GetTrafficLight
DROP PROCEDURE IF EXISTS GetTrafficLight;
CREATE PROCEDURE GetTrafficLight(
    IN ipDec INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED,
    IN curUnixTimeJIT INT UNSIGNED
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    SELECT
        description,
        IF(
            code = 'DRSE',
            80,
            IF(
                code = 'DRSD',
                20,
                IF(
                    (
                        SELECT COUNT(*)
                        FROM EventUpdate
                        WHERE
                            ipDecimal = ipDec
                            AND beginUnixTime = beginUnixTimeJIT
                            AND curUnixTime <= curUnixTimeJIT
                            AND code = 'LGOT'
                    ) = 1,
                    NULL,
                    20
                )
            )
        ) AS `颜色`
    FROM EventUpdate
    WHERE
        ipDecimal = ipDec
        AND beginUnixTime = beginUnixTimeJIT
        AND curUnixTime <= curUnixTimeJIT
        AND code IN ('STLG', 'DRSE', 'DRSD');
END;

-- GetSpeedTrap
DROP PROCEDURE IF EXISTS GetSpeedTrap;
CREATE PROCEDURE GetSpeedTrap(
    IN ipDec INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED,
    IN curUnixTimeJIT INT UNSIGNED
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    SELECT
        CONCAT(ROW_NUMBER() OVER (ORDER BY Speed DESC), '-', driverName) AS `NAME`,
        Speed
    FROM (
        SELECT
            driverName,
            MAX(CAST(SUBSTRING(description, -9, 5) AS UNSIGNED)) AS Speed
        FROM EventUpdate
        WHERE
            ipDecimal = ipDec
            AND beginUnixTime = beginUnixTimeJIT
            AND curUnixTime <= curUnixTimeJIT
            AND code = 'SPTP'
        GROUP BY driverName
    ) AS t
    ORDER BY Speed DESC;
END;