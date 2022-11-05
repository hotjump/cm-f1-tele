#pragma once

#include <inttypes.h>
#include <cstdint>
#include <sstream>

#include "common/enum_macro.h"
#include "common/time_format.h"

typedef uint8_t uint8;
typedef int8_t int8;
typedef uint16_t uint16;
typedef int16_t int16;
typedef uint64_t uint64;
typedef uint32_t uint32;

ENUM(PacketID, CarMotion, SessionData, LapData, EventData, Participants, CarSetup, CarTelemetry, CarStatus,
     FinalClassification, LobbyInfo, CarDamage, SessionHistory, Count);

ENUM(ResultStatus, invalid, inactive, active, finished, didnotfinish, disqualified, not_classified, retired);

ENUM(Track, Melbourne, Paul_Ricard, Shanghai, Sakhir, Catalunya, Monaco, Montreal, Silverstone, Hockenheim, Hungaroring,
     Spa, Monza, Singapore, Suzuka, Abu_Dhabi, Texas, Brazil, Austria, Sochi, Mexico, Baku, Sakhir_Short,
     Silverstone_Short, Texas_Short, Suzuka_Short, Hanoi, Zandvoort, Imola, Portimao, Jeddah, Miami);

#define FuntionCommonArg uint32_t ip, uint32_t begin, uint32_t current
#define PrimaryKeyCommonPart ip, begin, current

#pragma pack(push, 1)

struct PacketHeader {
  uint16 m_packetFormat;            // 2022
  uint8 m_gameMajorVersion;         // Game major version - "X.00"
  uint8 m_gameMinorVersion;         // Game minor version - "1.XX"
  uint8 m_packetVersion;            // Version of this packet type, all start from 1
  uint8 m_packetId;                 // Identifier for the packet type, see below
  uint64 m_sessionUID;              // Unique identifier for the session
  float m_sessionTime;              // Session timestamp
  uint32 m_frameIdentifier;         // Identifier for the frame the data was retrieved on
  uint8 m_playerCarIndex;           // Index of player's car in the array
  uint8 m_secondaryPlayerCarIndex;  // Index of secondary player's car in the array (splitscreen)
                                    // 255 if no second player
  std::string ToSQL(FuntionCommonArg) const {
    char stmt[128] = {0};
    const char* fmt = "INSERT INTO SessionList Values(%u,%u,%u,now(),%f,%u,%u,%u,%u,%u,%u,%u,%u);\n";
    snprintf(stmt, sizeof(stmt), fmt, PrimaryKeyCommonPart, m_sessionTime, m_sessionUID, m_frameIdentifier,
             m_packetFormat, m_gameMajorVersion, m_gameMinorVersion, m_packetVersion, m_playerCarIndex,
             m_secondaryPlayerCarIndex);
    return std::string(stmt);
  }

  std::string AddUpdate(FuntionCommonArg) {
    char stmt[128] = {0};
    const char* fmt = "UPDATE SessionList SET curUnixTime=%u WHERE ipDecimal=%u AND beginUnixTime=%u;\n";
    snprintf(stmt, sizeof(stmt), fmt, current, ip, begin);

    return std::string(stmt);
  }
};

#pragma pack(pop)
