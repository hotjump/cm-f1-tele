DROP PROCEDURE IF EXISTS GetLobby;
CREATE PROCEDURE GetLobby(
    IN ipDec INT UNSIGNED,
    IN curUnixTimeJIT INT UNSIGNED
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    SELECT
        driverName,
        teamName,
        carNumber,
        readyStatusStr
    FROM LobbyInfo
    WHERE
        ipDecimal = ipDec
        AND curUnixTime = curUnixTimeJIT
    ORDER BY teamId;
END;