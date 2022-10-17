#pragma once

#include "common/enum_macro.h"
#include "packet_header.h"

/*
This packet details car statuses for all the cars in the race.

Frequency: Rate as specified in menus
Size: 1058 bytes
Version: 1
*/

ENUM(ActualTyreCompound, None0, None1, None2, None3, None4, None5, None6, INTERMEDIATE, WET, F1_CLASSIC_DRY,
     F1_CLASSIC_WET, F2_SUPERSOFT, F2_SOFT, F2_MEDIUM, F2_HARD, F2_WET, F1_C5, F1_C4, F1_C3, F1_C2, F1_C1);

ENUM(VisualTyreCompound, None0, None1, None2, None3, None4, None5, None6, INTERMEDIATE, WET, F1_CLASSIC_DRY,
     F1_CLASSIC_WET, None11, None12, None13, None14, F2_WET, F1_SOFT, F1_MEDIUM, F1_HARD, F2_SUPERSOFT, F2_SOFT,
     F2_MEDIUM, F2_HARD);

ENUM(FuelMix, lean, standard, rich, max);

ENUM(VehicleFiaFlags, none, green, blue, yellow, red);

ENUM(ErsDeployMode, none, medium, hotlap, overtake);

#pragma pack(push, 1)
struct CarStatusData {
  uint8 m_tractionControl;          // Traction control - 0 = off, 1 = medium, 2 = full
  uint8 m_antiLockBrakes;           // 0 (off) - 1 (on)
  uint8 m_fuelMix;                  // Fuel mix - 0 = lean, 1 = standard, 2 = rich, 3 = max
  uint8 m_frontBrakeBias;           // Front brake bias (percentage)
  uint8 m_pitLimiterStatus;         // Pit limiter status - 0 = off, 1 = on
  float m_fuelInTank;               // Current fuel mass
  float m_fuelCapacity;             // Fuel capacity
  float m_fuelRemainingLaps;        // Fuel remaining in terms of laps (value on MFD)
  uint16 m_maxRPM;                  // Cars max RPM, point of rev limiter
  uint16 m_idleRPM;                 // Cars idle RPM
  uint8 m_maxGears;                 // Maximum number of gears
  uint8 m_drsAllowed;               // 0 = not allowed, 1 = allowed
  uint16 m_drsActivationDistance;   // 0 = DRS not available, non-zero - DRS will be available
                                    // in [X] metres
  uint8 m_actualTyreCompound;       // F1 Modern - 16 = C5, 17 = C4, 18 = C3, 19 = C2, 20 = C1
                                    // 7 = inter, 8 = wet
                                    // F1 Classic - 9 = dry, 10 = wet
                                    // F2 – 11 = super soft, 12 = soft, 13 = medium, 14 = hard
                                    // 15 = wet
  uint8 m_visualTyreCompound;       // F1 visual (can be different from actual compound)
                                    // 16 = soft, 17 = medium, 18 = hard, 7 = inter, 8 = wet
                                    // F1 Classic – same as above
                                    // F2 ‘19, 15 = wet, 19 – super soft, 20 = soft
                                    // 21 = medium , 22 = hard
  uint8 m_tyresAgeLaps;             // Age in laps of the current set of tyres
  int8 m_vehicleFiaFlags;           // -1 = invalid/unknown, 0 = none, 1 = green
                                    // 2 = blue, 3 = yellow, 4 = red
  float m_ersStoreEnergy;           // ERS energy store in Joules
  uint8 m_ersDeployMode;            // ERS deployment mode, 0 = none, 1 = medium
                                    // 2 = hotlap, 3 = overtake
  float m_ersHarvestedThisLapMGUK;  // ERS energy harvested this lap by MGU-K
  float m_ersHarvestedThisLapMGUH;  // ERS energy harvested this lap by MGU-H
  float m_ersDeployedThisLap;       // ERS energy deployed this lap
  uint8 m_networkPaused;            // Whether the car is paused in a network game
};

struct PacketCarStatusData {
  PacketHeader m_header;  // Header

  CarStatusData m_carStatusData[22];

  std::string ToSQL(uint32_t begin, uint32_t current, uint8 dirver_num, const ParticipantData* driver_name) const {
    std::string sql;
    sql.reserve(4 * 1024);

    char stmt[512] = {0};
    const CarStatusData* p = m_carStatusData;
    const char* fmt =
        "INSERT INTO CarStatus "
        "Values(%u,%u,NOW(),%u,'%s',%u,%u,%u,'%s',%u,%u,%f,%f,%f,%u,%u,%u,%u,%u,%u,%u,%u,%i,'%s',%f,%u,'%s',%f,%f,%f,%"
        "u);\n";
    for (uint8 i = 0; i < dirver_num; i++) {
      snprintf(stmt, sizeof(stmt), fmt, begin, current, i + 1, driver_name[i].m_name, p[i].m_tractionControl,
               p[i].m_antiLockBrakes, p[i].m_fuelMix, EnumToCStr(FuelMix, p[i].m_fuelMix), p[i].m_frontBrakeBias,
               p[i].m_pitLimiterStatus, p[i].m_fuelInTank, p[i].m_fuelCapacity, p[i].m_fuelRemainingLaps, p[i].m_maxRPM,
               p[i].m_idleRPM, p[i].m_maxGears, p[i].m_drsAllowed, p[i].m_drsActivationDistance,
               p[i].m_actualTyreCompound, p[i].m_visualTyreCompound, p[i].m_tyresAgeLaps, p[i].m_vehicleFiaFlags,
               EnumToCStr(VehicleFiaFlags, p[i].m_vehicleFiaFlags), p[i].m_ersStoreEnergy, p[i].m_ersDeployMode,
               EnumToCStr(ErsDeployMode, p[i].m_ersDeployMode), p[i].m_ersHarvestedThisLapMGUK,
               p[i].m_ersHarvestedThisLapMGUH, p[i].m_ersDeployedThisLap, p[i].m_networkPaused);
      sql += stmt;
    }

    return sql;
  }
};
#pragma pack(pop)