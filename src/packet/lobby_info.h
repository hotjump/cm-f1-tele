#pragma once

#include "packet_header.h"

/*
This packet details the players currently in a multiplayer lobby. It details each player’s selected car, any AI involved
in the game and also the ready status of each of the participants.

Frequency: Two every second when in the lobby
Size: 1191 bytes
Version: 1
*/

ENUM(ReadyStatus, not_ready, ready, spectating);

#pragma pack(push, 1)

struct LobbyInfoData {
  uint8 m_aiControlled;  // Whether the vehicle is AI (1) or Human (0) controlled
  uint8 m_teamId;        // Team id - see appendix (255 if no team currently selected)
  uint8 m_nationality;   // Nationality of the driver
  char m_name[48];       // Name of participant in UTF-8 format – null terminated
                         // Will be truncated with ... (U+2026) if too long
  uint8 m_carNumber;     // Car number of the player
  uint8 m_readyStatus;   // 0 = not ready, 1 = ready, 2 = spectating

  std::string name() const {
    if (strcmp(m_name, "player") == 0 || strcmp(m_name, "玩家") == 0) {
      return std::string(MapToCStr(TeamName, m_teamId, "unknown")) + std::string("-") + std::to_string(m_carNumber);
    } else {
      return std::string(m_name);
    }
  }
};

struct PacketLobbyInfoData {
  PacketHeader m_header;  // Header

  // Packet specific data
  uint8 m_numPlayers;  // Number of players in the lobby data
  LobbyInfoData m_lobbyPlayers[22];

  std::string ToSQL(uint32_t ip, uint32_t current) const {
    std::string sql;
    sql.reserve(4 * 1024);
    char stmt[512] = {0};
    const LobbyInfoData* p = m_lobbyPlayers;
    const char* fmt = "REPLACE INTO LobbyInfo Values(%u,%u,now(),%u,'%s',%u,%u,'%s',%u,%u,%u,'%s');\n";
    for (uint8 i = 0; i < m_numPlayers; i++) {
      snprintf(stmt, sizeof(stmt), fmt, ip, current, i + 1, p[i].name().c_str(), p[i].m_aiControlled, p[i].m_teamId,
               MapToCStr(TeamName, p[i].m_teamId, "unknown"), p[i].m_nationality, p[i].m_carNumber, p[i].m_readyStatus,
               EnumToCStr(ReadyStatus, p[i].m_readyStatus));
      sql += stmt;
    }

    return sql;
  }
};

#pragma pack(pop)
