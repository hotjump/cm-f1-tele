#pragma once

#include <map>
#include <vector>

#include "packet/session.h"
#include "packet_header.h"

/*
This is a list of participants in the race. If the vehicle is controlled by AI, then the name will be the driver name.
If this is a multiplayer game, the names will be the Steam Id on PC, or the LAN name if appropriate.

N.B. on Xbox One, the names will always be the driver name, on PS4 the name will be the LAN name if playing a LAN game,
otherwise it will be the driver name.

The array should be indexed by vehicle index.

Frequency: Every 5 seconds
Size: 1257 bytes
Version: 1
*/

ENUM(AIControlled, Human, AI);

const std::map<uint8, const char*> TeamName = {
    {0, "Mercedes"},
    {1, "Ferrari"},
    {2, "Red Bull Racing"},
    {3, "Williams"},
    {4, "Aston Martin"},
    {5, "Alpine"},
    {6, "Alpha Tauri"},
    {7, "Haas"},
    {8, "McLaren"},
    {9, "Alfa Romeo"},
    {85, "Mercedes 2020"},
    {86, "Ferrari 2020"},
    {87, "Red Bull 2020"},
    {88, "Williams 2020"},
    {89, "Racing Point 2020"},
    {90, "Renault 2020"},
    {91, "Alpha Tauri 2020"},
    {92, "Haas 2020"},
    {93, "McLaren 2020"},
    {94, "Alfa Romeo 2020"},
    {95, "Aston Martin DB11 V12"},
    {96, "Aston Martin Vantage F1 Edition"},
    {97, "Aston Martin Vantage Safety Car"},
    {98, "Ferrari F8 Tributo"},
    {99, "Ferrari Roma"},
    {100, "McLaren 720S"},
    {101, "McLaren Artura"},
    {102, "Mercedes AMG GT Black Series Safety Car"},
    {103, "Mercedes AMG GTR Pro"},
    {104, "F1 Custom Team"},
    {106, "Prema ‘21"},
    {107, "Uni-Virtuosi ‘21"},
    {108, "Carlin ‘21"},
    {109, "Hitech ‘21"},
    {110, "Art GP ‘21"},
    {111, "MP Motorsport ‘21"},
    {112, "Charouz ‘21"},
    {113, "Dams ‘21"},
    {114, "Campos ‘21"},
    {115, "BWT ‘21"},
    {116, "Trident ‘21"},
    {117, "Mercedes AMG GT Black Series"},
    {118, "Dream Team"},  // doc is not existed, but it existed.
    {255, "No selected"},
};

const std::vector<std::string> Nationality = {
    "Unkown",         "American",   "Argentinean", "Australian",  "Austrian",   "Azerbaijani",   "Bahraini",
    "Belgian",        "Bolivian",   "Brazilian",   "British",     "Bulgarian",  "Cameroonian",   "Canadian",
    "Chilean",        "Chinese",    "Colombian",   "Costa Rican", "Croatian",   "Cypriot",       "Czech",
    "Danish",         "Dutch",      "Ecuadorian",  "English",     "Emirian",    "Estonian",      "Finnish",
    "French",         "German",     "Ghanaian",    "Greek",       "Guatemalan", "Honduran",      "Hong Konger",
    "Hungarian",      "Icelander",  "Indian",      "Indonesian",  "Irish",      "Israeli",       "Italian",
    "Jamaican",       "Japanese",   "Jordanian",   "Kuwaiti",     "Latvian",    "Lebanese",      "Lithuanian",
    "Luxembourger",   "Malaysian",  "Maltese",     "Mexican",     "Monegasque", "New Zealander", "Nicaraguan",
    "Northern Irish", "Norwegian",  "Omani",       "Pakistani",   "Panamanian", "Paraguayan",    "Peruvian",
    "Polish",         "Portuguese", "Qatari",      "Romanian",    "Russian",    "Salvadoran",    "Saudi",
    "Scottish",       "Serbian",    "Singaporean", "Slovakian",   "Slovenian",  "South Korean",  "South African",
    "Spanish",        "Swedish",    "Swiss",       "Thai",        "Turkish",    "Uruguayan",     "Ukrainian",
    "Venezuelan",     "Barbadian",  "Welsh",       "Vietnamese",
};

#define ParticipantDataArg uint8_t dirver_num, const ParticipantData *driver_name

#pragma pack(push, 1)

struct ParticipantData {
  uint8 m_aiControlled;   // Whether the vehicle is AI (1) or Human (0) controlled
  uint8 m_driverId;       // Driver id - see appendix, 255 if network human
  uint8 m_networkId;      // Network id – unique identifier for network players
  uint8 m_teamId;         // Team id - see appendix
  uint8 m_myTeam;         // My team flag – 1 = My Team, 0 = otherwise
  uint8 m_raceNumber;     // Race number of the car
  uint8 m_nationality;    // Nationality of the driver
  char m_name[48];        // Name of participant in UTF-8 format – null terminated
                          // Will be truncated with … (U+2026) if too long
  uint8 m_yourTelemetry;  // The player's UDP setting, 0 = restricted, 1 = public

  std::string name() const {
    if (strcmp(m_name, "player") == 0 || strcmp(m_name, "玩家") == 0) {
      return std::string(TeamName.at(m_teamId)) + std::string("-") + std::to_string(m_raceNumber);
    } else {
      return std::string(m_name);
    }
  }
};

struct PacketParticipantsData {
  PacketHeader m_header;  // Header

  uint8 m_numActiveCars;  // Number of active cars in the data – should match number of
                          // cars on HUD
  ParticipantData m_participants[22];

  uint8 DriverNum(const PacketSessionData* session) {
    uint8 count = 0;
    const ParticipantData* p = m_participants;
    for (uint8 i = 0; i < (sizeof(m_participants) / sizeof(ParticipantData)); i++) {
      if (p[i].m_teamId == 255 ||
          (session->m_networkGame == 1 && p[i].m_networkId == 255 && p[i].m_aiControlled == 0)) {
        continue;
      }
      count++;
    }
    return count;
  }

  std::string ToSQL(FuntionCommonArg) const {
    std::string sql;
    sql.reserve(4 * 1024);
    sql += "REPLACE INTO Participants Values\n";
    const char* fmt = "(%u,%u,%u,now(),%u,'%s',%u,%u,%u,%u,'%s',%u,%u,%u,'%s',%u),\n";
    char stmt[512] = {0};
    const ParticipantData* p = m_participants;
    auto num_participants = static_cast<uint8>(sizeof(m_participants) / sizeof(ParticipantData));
    for (uint8 i = 0; i < num_participants; i++) {
      if (p[i].m_teamId == 255) {
        continue;
      }
      snprintf(stmt, sizeof(stmt), fmt, PrimaryKeyCommonPart, i + 1, p[i].name().c_str(), p[i].m_aiControlled,
               p[i].m_driverId, p[i].m_networkId, p[i].m_teamId,
               (p[i].m_teamId == 255 ? "-" : TeamName.at(p[i].m_teamId)), p[i].m_myTeam, p[i].m_raceNumber,
               p[i].m_nationality, (p[i].m_nationality == 255 ? "-" : Nationality[p[i].m_nationality].c_str()),
               p[i].m_yourTelemetry);
      sql += stmt;
    }
    sql[sql.size() - 2] = ';';

    return sql;
  }
};

#pragma pack(pop)
