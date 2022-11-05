#pragma once

#include "packet_header.h"

/*
This packet details the car setups for each vehicle in the session. Note that in multiplayer games, other player cars
will appear as blank, you will only be able to see your car setup and AI cars.

Frequency: 2 per second
Size: 1102 bytes
Version: 1
*/
#pragma pack(push, 1)

struct CarSetupData {
  uint8 m_frontWing;               // Front wing aero
  uint8 m_rearWing;                // Rear wing aero
  uint8 m_onThrottle;              // Differential adjustment on throttle (percentage)
  uint8 m_offThrottle;             // Differential adjustment off throttle (percentage)
  float m_frontCamber;             // Front camber angle (suspension geometry)
  float m_rearCamber;              // Rear camber angle (suspension geometry)
  float m_frontToe;                // Front toe angle (suspension geometry)
  float m_rearToe;                 // Rear toe angle (suspension geometry)
  uint8 m_frontSuspension;         // Front suspension
  uint8 m_rearSuspension;          // Rear suspension
  uint8 m_frontAntiRollBar;        // Front anti-roll bar
  uint8 m_rearAntiRollBar;         // Front anti-roll bar
  uint8 m_frontSuspensionHeight;   // Front ride height
  uint8 m_rearSuspensionHeight;    // Rear ride height
  uint8 m_brakePressure;           // Brake pressure (percentage)
  uint8 m_brakeBias;               // Brake bias (percentage)
  float m_rearLeftTyrePressure;    // Rear left tyre pressure (PSI)
  float m_rearRightTyrePressure;   // Rear right tyre pressure (PSI)
  float m_frontLeftTyrePressure;   // Front left tyre pressure (PSI)
  float m_frontRightTyrePressure;  // Front right tyre pressure (PSI)
  uint8 m_ballast;                 // Ballast
  float m_fuelLoad;                // Fuel load
};

struct PacketCarSetupData {
  PacketHeader m_header;  // Header

  CarSetupData m_carSetups[22];

  std::string ToSQL(FuntionCommonArg, ParticipantDataArg) const {
    std::string sql;

    char stmt[512] = {0};
    const CarSetupData* p = m_carSetups;
    const char* fmt =
        "REPLACE INTO CarSetup "
        "VALUES(%u,%u,%u,NOW(),%u,'%s',%u,%u,%u,%u,%.1f,%.1f,%.1f,%.1f,%u,%u,%u,%u,%u,%u,%u,%u,%.1f,%.1f,%.1f,%.1f,%u,%"
        ".1f);"
        "\n";

    for (uint8 i = 0; i < dirver_num; i++) {
      snprintf(stmt, sizeof(stmt), fmt, PrimaryKeyCommonPart, i + 1, driver_name[i].name().c_str(), p[i].m_frontWing,
               p[i].m_rearWing, p[i].m_onThrottle, p[i].m_offThrottle, p[i].m_frontCamber, p[i].m_rearCamber,
               p[i].m_frontToe, p[i].m_rearToe, p[i].m_frontSuspension, p[i].m_rearSuspension, p[i].m_frontAntiRollBar,
               p[i].m_rearAntiRollBar, p[i].m_frontSuspensionHeight, p[i].m_rearSuspensionHeight, p[i].m_brakePressure,
               p[i].m_brakeBias, p[i].m_rearLeftTyrePressure, p[i].m_rearRightTyrePressure,
               p[i].m_frontLeftTyrePressure, p[i].m_frontRightTyrePressure, p[i].m_ballast, p[i].m_fuelLoad);
      sql += stmt;
    }
    return sql;
  }
};

#pragma pack(pop)
