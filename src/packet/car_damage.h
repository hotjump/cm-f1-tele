#pragma once

#include "packet.h"
#include "packet_header.h"
#include "participants.h"

/*
This packet details car damage parameters for all the cars in the race.

Frequency: 2 per second
Size: 948 bytes
Version: 1
*/
#pragma pack(push, 1)

struct CarDamageData {
  float m_tyresWear[4];          // Tyre wear (percentage)
  uint8 m_tyresDamage[4];        // Tyre damage (percentage)
  uint8 m_brakesDamage[4];       // Brakes damage (percentage)
  uint8 m_frontLeftWingDamage;   // Front left wing damage (percentage)
  uint8 m_frontRightWingDamage;  // Front right wing damage (percentage)
  uint8 m_rearWingDamage;        // Rear wing damage (percentage)
  uint8 m_floorDamage;           // Floor damage (percentage)
  uint8 m_diffuserDamage;        // Diffuser damage (percentage)
  uint8 m_sidepodDamage;         // Sidepod damage (percentage)
  uint8 m_drsFault;              // Indicator for DRS fault, 0 = OK, 1 = fault
  uint8 m_ersFault;              // Indicator for ERS fault, 0 = OK, 1 = fault
  uint8 m_gearBoxDamage;         // Gear box damage (percentage)
  uint8 m_engineDamage;          // Engine damage (percentage)
  uint8 m_engineMGUHWear;        // Engine wear MGU-H (percentage)
  uint8 m_engineESWear;          // Engine wear ES (percentage)
  uint8 m_engineCEWear;          // Engine wear CE (percentage)
  uint8 m_engineICEWear;         // Engine wear ICE (percentage)
  uint8 m_engineMGUKWear;        // Engine wear MGU-K (percentage)
  uint8 m_engineTCWear;          // Engine wear TC (percentage)
  uint8 m_engineBlown;           // Engine blown, 0 = OK, 1 = fault
  uint8 m_engineSeized;          // Engine seized, 0 = OK, 1 = fault
};

struct PacketCarDamageData {
  PacketHeader m_header;  // Header

  CarDamageData m_carDamageData[22];

  std::string ToSQL(FuntionCommonArg, ParticipantDataArg, TTArg) const {
    std::string sql;
    sql += "INSERT INTO CarDamage VALUES\n";
    const char* fmt =
        "(%u,%u,%u,NOW(),%u,'%s',%.1f,%.1f,%.1f,%.1f,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%"
        "u,%u,%u,%u,%u,%u,%u),\n";
    char stmt[512] = {0};
    const CarDamageData* p = m_carDamageData;
    for (uint8 i = 0; i < dirver_num; i++) {
      SkipTTNonexistedParticipant;
      snprintf(stmt, sizeof(stmt), fmt, PrimaryKeyCommonPart, i + 1,
               driver_name[i].name().c_str(),                                                       //
               p[i].m_tyresWear[0], p[i].m_tyresWear[1], p[i].m_tyresWear[2], p[i].m_tyresWear[3],  // typr werar
               p[i].m_tyresDamage[0], p[i].m_tyresDamage[1], p[i].m_tyresDamage[2], p[i].m_tyresDamage[3],
               p[i].m_brakesDamage[0], p[i].m_brakesDamage[1], p[i].m_brakesDamage[2], p[i].m_brakesDamage[3],
               p[i].m_frontLeftWingDamage, p[i].m_frontRightWingDamage, p[i].m_rearWingDamage, p[i].m_floorDamage,
               p[i].m_diffuserDamage, p[i].m_sidepodDamage, p[i].m_drsFault, p[i].m_ersFault, p[i].m_gearBoxDamage,
               p[i].m_engineDamage, p[i].m_engineMGUHWear, p[i].m_engineESWear, p[i].m_engineCEWear,
               p[i].m_engineICEWear, p[i].m_engineMGUKWear, p[i].m_engineTCWear, p[i].m_engineBlown,
               p[i].m_engineSeized);
      sql += stmt;
    }

    sql[sql.size() - 2] = ';';
    return sql;
  }
};

#pragma pack(pop)
