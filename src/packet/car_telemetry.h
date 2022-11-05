#pragma once
#include "packet_header.h"

/*
This packet details telemetry for all the cars in the race. It details various values that would be recorded on the car
such as speed, throttle application, DRS etc. Note that the rev light configurations are presented separately as well
and will mimic real life driver preferences.

Frequency: Rate as specified in menus
Size: 1347 bytes
Version: 1
*/

ENUM(SurfaceType, Tarmac, Rumble_strip, Concrete, Rock, Gravel, Mud, Sand, Grass, Water, Cobblestone, Metal, Ridged);
#pragma pack(push, 1)
struct CarTelemetryData {
  uint16 m_speed;                      // Speed of car in kilometres per hour
  float m_throttle;                    // Amount of throttle applied (0.0 to 1.0)
  float m_steer;                       // Steering (-1.0 (full lock left) to 1.0 (full lock right))
  float m_brake;                       // Amount of brake applied (0.0 to 1.0)
  uint8 m_clutch;                      // Amount of clutch applied (0 to 100)
  int8 m_gear;                         // Gear selected (1-8, N=0, R=-1)
  uint16 m_engineRPM;                  // Engine RPM
  uint8 m_drs;                         // 0 = off, 1 = on
  uint8 m_revLightsPercent;            // Rev lights indicator (percentage)
  uint16 m_revLightsBitValue;          // Rev lights (bit 0 = leftmost LED, bit 14 = rightmost LED)
  uint16 m_brakesTemperature[4];       // Brakes temperature (celsius)
  uint8 m_tyresSurfaceTemperature[4];  // Tyres surface temperature (celsius)
  uint8 m_tyresInnerTemperature[4];    // Tyres inner temperature (celsius)
  uint16 m_engineTemperature;          // Engine temperature (celsius)
  float m_tyresPressure[4];            // Tyres pressure (PSI)
  uint8 m_surfaceType[4];              // Driving surface, see appendices
};

struct PacketCarTelemetryData {
  PacketHeader m_header;  // Header

  CarTelemetryData m_carTelemetryData[22];

  uint8 m_mfdPanelIndex;                 // Index of MFD panel open - 255 = MFD closed
                                         // Single player, race – 0 = Car setup, 1 = Pits
                                         // 2 = Damage, 3 =  Engine, 4 = Temperatures
                                         // May vary depending on game mode
  uint8 m_mfdPanelIndexSecondaryPlayer;  // See above
  int8 m_suggestedGear;                  // Suggested gear for the player (1-8)
                                         // 0 if no gear suggested

  std::string ToSQL(FuntionCommonArg, ParticipantDataArg) const {
    std::string sql;
    sql.reserve(4 * 1024);

    char stmt[512] = {0};
    const CarTelemetryData* p = m_carTelemetryData;
    sql += "INSERT INTO CarTelemetry Values\n";
    const char* fmt =
        "(%u,%u,%u,NOW(),%u,'%s',%u,%.2f,%.2f,%.2f,%u,%i,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%.1f,"
        "%.1f,%.1f,%.1f,%u,%u,%u,%u)%c\n";
    for (uint8 i = 0; i < dirver_num; i++) {
      snprintf(stmt, sizeof(stmt), fmt, PrimaryKeyCommonPart, i + 1, driver_name[i].name().c_str(), p[i].m_speed,
               p[i].m_throttle, p[i].m_steer, p[i].m_brake, p[i].m_clutch, p[i].m_gear, p[i].m_engineRPM, p[i].m_drs,
               p[i].m_revLightsPercent, p[i].m_revLightsBitValue, p[i].m_brakesTemperature[0],
               p[i].m_brakesTemperature[1], p[i].m_brakesTemperature[2], p[i].m_brakesTemperature[3],
               p[i].m_tyresSurfaceTemperature[0], p[i].m_tyresSurfaceTemperature[1], p[i].m_tyresSurfaceTemperature[2],
               p[i].m_tyresSurfaceTemperature[3], p[i].m_tyresInnerTemperature[0], p[i].m_tyresInnerTemperature[1],
               p[i].m_tyresInnerTemperature[2], p[i].m_tyresInnerTemperature[3], p[i].m_engineTemperature,
               p[i].m_tyresPressure[0], p[i].m_tyresPressure[1], p[i].m_tyresPressure[2], p[i].m_tyresPressure[3],
               p[i].m_surfaceType[0], p[i].m_surfaceType[1], p[i].m_surfaceType[2], p[i].m_surfaceType[3],
               i + 1 == dirver_num ? ';' : ',');
      sql += stmt;
    }

    return sql;
  }
};
#pragma pack(pop)