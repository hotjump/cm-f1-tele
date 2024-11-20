CREATE PROCEDURE GetParticipants(
    in ipDec int unsigned,
    in beginUnixTimeJIT int unsigned,
    in curUnixTimeJIT int unsigned
)
select
    carIndex,
    driverName,
    teamName,
    yourTelemetry,
    aiControlled,
    networkId
from
    Participants
WHERE
    ipDecimal = ipDec
    AND beginUnixTime = beginUnixTimeJIT
    AND curUnixTime = curUnixTimeJIT
order by
    teamId;