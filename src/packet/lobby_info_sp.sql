CREATE PROCEDURE GetLobby(
    in ipDec int unsigned,
    in curUnixTimeJIT int unsigned
)
select
    driverName,
    teamName,
    carNumber,
    readyStatusStr
from
    LobbyInfo
WHERE
    ipDecimal = ipDec
    AND curUnixTime = curUnixTimeJIT
order by
    teamId;