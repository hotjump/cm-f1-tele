CREATE PROCEDURE GetTrafficLight(
    in ipDec int unsigned,
    in beginUnixTimeJIT int unsigned,
    in curUnixTimeJIT int unsigned
)
select
    description,
    if (
        code = 'DRSE',
        80,
        if(
            code = 'DRSD',
            20,
            if(
                (
                    select
                        count(*)
                    from
                        EventUpdate
                    WHERE
                        ipDecimal = ipDec
                        AND beginUnixTime = beginUnixTimeJIT
                        AND curUnixTime <= curUnixTimeJIT
                        AND code = "LGOT"
                ) = 1,
                NULL,
                20
            )
        )
    ) AS "颜色"
from
    EventUpdate
WHERE
    ipDecimal = ipDec
    AND beginUnixTime = beginUnixTimeJIT
    AND curUnixTime <= curUnixTimeJIT
    AND code IN ("STLG", "DRSE", "DRSD");

CREATE PROCEDURE GetSpeedTrap(
    in ipDec int unsigned,
    in beginUnixTimeJIT int unsigned,
    in curUnixTimeJIT int unsigned
)
SELECT
    CONCAT((@i := @i + 1), "-", driverName) AS NAME,
    Speed
FROM
    (
        select
            driverName,
            MAX(SUBSTRING(description, -9, 5) + 0) AS "Speed"
        from
            EventUpdate
        WHERE
            ipDecimal = ipDec
            AND beginUnixTime = beginUnixTimeJIT
            AND curUnixTime <= curUnixTimeJIT
            AND code = "SPTP"
        GROUP BY
            driverName
        Order by
            Speed desc
    ) AS t,
    (
        SELECT
            @i := 0
    ) as i;