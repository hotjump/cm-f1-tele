DROP PROCEDURE IF EXISTS GetParticipants;
CREATE PROCEDURE GetParticipants(
    IN ipDec INT UNSIGNED,
    IN beginUnixTimeJIT INT UNSIGNED,
    IN curUnixTimeJIT INT UNSIGNED
)
READS SQL DATA
SQL SECURITY INVOKER
BEGIN
    SELECT
        carIndex,
        driverName,
        teamName,
        yourTelemetry,
        aiControlled,
        networkId
    FROM Participants
    WHERE
        ipDecimal = ipDec
        AND beginUnixTime = beginUnixTimeJIT
        AND curUnixTime = curUnixTimeJIT
    ORDER BY teamId;
END;