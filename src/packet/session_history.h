#pragma once
#include "car_status.h"
#include "common/time_format.h"
#include "packet_header.h"

/*
This packet contains lap times and tyre usage for the session. This packet works slightly differently to other packets.
To reduce CPU and bandwidth, each packet relates to a specific vehicle and is sent every 1/20 s, and the vehicle being
sent is cycled through. Therefore in a 20 car race you should receive an update for each vehicle at least once per
second.

Note that at the end of the race, after the final classification packet has been sent, a final bulk update of all the
session histories for the vehicles in that session will be sent.

Frequency: 20 per second but cycling through cars
Size: 1155 bytes
Version: 1
*/

#pragma pack(push, 1)

struct LapHistoryData {
  uint32 m_lapTimeInMS;      // Lap time in milliseconds
  uint16 m_sector1TimeInMS;  // Sector 1 time in milliseconds
  uint16 m_sector2TimeInMS;  // Sector 2 time in milliseconds
  uint16 m_sector3TimeInMS;  // Sector 3 time in milliseconds
  uint8 m_lapValidBitFlags;  // 0x01 bit set-lap valid,      0x02 bit set-sector 1 valid
                             // 0x04 bit set-sector 2 valid, 0x08 bit set-sector 3 valid
};

struct TyreStintHistoryData {
  uint8 m_endLap;              // Lap the tyre usage ends on (255 of current tyre)
  uint8 m_tyreActualCompound;  // Actual tyres used by this driver
  uint8 m_tyreVisualCompound;  // Visual tyres used by this driver
};

struct PacketSessionHistoryData {
  PacketHeader m_header;  // Header

  uint8 m_carIdx;         // Index of the car this lap data relates to
  uint8 m_numLaps;        // Num laps in the data (including current partial lap)
  uint8 m_numTyreStints;  // Number of tyre stints in the data

  uint8 m_bestLapTimeLapNum;  // Lap the best lap time was achieved on
  uint8 m_bestSector1LapNum;  // Lap the best Sector 1 time was achieved on
  uint8 m_bestSector2LapNum;  // Lap the best Sector 2 time was achieved on
  uint8 m_bestSector3LapNum;  // Lap the best Sector 3 time was achieved on

  LapHistoryData m_lapHistoryData[100];  // 100 laps of data max
  TyreStintHistoryData m_tyreStintsHistoryData[8];

  std::string ToSQL(FuntionCommonArg, ParticipantDataArg, TTArg) const {
    if (is_tt && !(m_carIdx == 0 || m_carIdx == timeTrialPBCarIdx || m_carIdx == timeTrialRivalCarIdx)) {
      return std::string();
    }

    char stmt[512] = {0};
    std::string sql;

    sql += "REPLACE INTO LapHistoryData Values\n";
    const char* fmt_lap_history_data =
        "(%u,%u,%u,now(),%u,'%s',%u,%u,'%s',%u,'%s',%u,'%s',%u,'%s',%u,%u,'%s',%u,'%s',%u)%c\n";

    const LapHistoryData* p_l = m_lapHistoryData;
    const TyreStintHistoryData* p_s = m_tyreStintsHistoryData;
    uint8 current_stint = 0;
    uint8 tyre_lap_num_used = 0;

    for (uint8 i = 0; i < m_numLaps; i++) {
      TimeFormat sector1TimeInMS(p_l[i].m_sector1TimeInMS);
      TimeFormat sector2TimeInMS(p_l[i].m_sector2TimeInMS);
      TimeFormat sector3TimeInMS(p_l[i].m_sector3TimeInMS);
      TimeFormat lapTimeInMS(p_l[i].m_lapTimeInMS);

      tyre_lap_num_used++;

      if (i + 1 > p_s[current_stint].m_endLap) {
        ++current_stint;
        tyre_lap_num_used = 0;
      }

      // todo: 这里carIdx需要看看是不是从0开始
      snprintf(stmt, sizeof(stmt), fmt_lap_history_data, PrimaryKeyCommonPart, m_carIdx + 1,
               driver_name[m_carIdx].name().c_str(), i + 1, p_l[i].m_sector1TimeInMS, sector1TimeInMS.c_str(),
               p_l[i].m_sector2TimeInMS, sector2TimeInMS.c_str(), p_l[i].m_sector3TimeInMS, sector3TimeInMS.c_str(),
               p_l[i].m_lapTimeInMS, lapTimeInMS.c_str(), p_l[i].m_lapValidBitFlags,
               p_s[current_stint].m_tyreActualCompound,
               EnumToCStr(ActualTyreCompound, p_s[current_stint].m_tyreActualCompound),
               p_s[current_stint].m_tyreVisualCompound,
               EnumToCStr(VisualTyreCompound, p_s[current_stint].m_tyreVisualCompound), tyre_lap_num_used,
               i + 1 == m_numLaps ? ';' : ',');
      sql += stmt;
    }

    const char* fmt_best_lap =
        "INSERT INTO BestLap "
        "Values(%u,%u,%u,now(),%u,'%s',%u,%u,'%s',%u,'%s',%u,'%s',%u,'%s',%u,%u,'%s',%u,%u,'%s',%u,%u,'%s',%u,'%s');\n";

    uint16 bestSector1TimeInMS = 0;         // Sector 1 time in milliseconds
    uint16 bestSector2TimeInMS = 0;         // Sector 2 time in milliseconds
    uint16 bestSector3TimeInMS = 0;         // Sector 3 time in milliseconds
    uint32 theoreticalBestLapTimeInMS = 0;  // Lap time in milliseconds

    if (m_bestSector1LapNum > 0 && m_bestSector1LapNum < 100) {
      bestSector1TimeInMS = p_l[m_bestSector1LapNum - 1].m_sector1TimeInMS;
    }

    if (m_bestSector2LapNum > 0 && m_bestSector2LapNum < 100) {
      bestSector2TimeInMS = p_l[m_bestSector2LapNum - 1].m_sector2TimeInMS;
    }

    if (m_bestSector3LapNum > 0 && m_bestSector3LapNum < 100) {
      bestSector3TimeInMS = p_l[m_bestSector3LapNum - 1].m_sector3TimeInMS;
    }

    if (bestSector3TimeInMS > 0 && bestSector2TimeInMS > 0 && bestSector1TimeInMS > 0) {
      theoreticalBestLapTimeInMS = bestSector3TimeInMS + bestSector2TimeInMS + bestSector1TimeInMS;
    }

    uint16 bestLapSector1TimeInMS = 0;  // Sector 1 time in milliseconds
    uint16 bestLapSector2TimeInMS = 0;  // Sector 2 time in milliseconds
    uint16 bestLapSector3TimeInMS = 0;  // Sector 3 time in milliseconds
    uint32 bestLapTimeInMS = 0;         // Lap time in milliseconds

    if (m_bestLapTimeLapNum > 0 && m_bestLapTimeLapNum < 100) {
      uint8 best_lap = m_bestLapTimeLapNum - 1;
      bestLapSector1TimeInMS = p_l[best_lap].m_sector1TimeInMS;
      bestLapSector2TimeInMS = p_l[best_lap].m_sector2TimeInMS;
      bestLapSector3TimeInMS = p_l[best_lap].m_sector3TimeInMS;
      bestLapTimeInMS = p_l[best_lap].m_lapTimeInMS;
    }

    TimeFormat bestLapSector1TimeInStr(bestLapSector1TimeInMS);
    TimeFormat bestLapSector2TimeInStr(bestLapSector2TimeInMS);
    TimeFormat bestLapSector3TimeInStr(bestLapSector3TimeInMS);
    TimeFormat bestLapTimeInStr(bestLapTimeInMS);

    TimeFormat bestSector1TimeInStr(bestSector1TimeInMS);
    TimeFormat bestSector2TimeInStr(bestSector2TimeInMS);
    TimeFormat bestSector3TimeInStr(bestSector3TimeInMS);
    TimeFormat theoreticalBestLapTimeInStr(theoreticalBestLapTimeInMS);

    snprintf(stmt, sizeof(stmt), fmt_best_lap, PrimaryKeyCommonPart, m_carIdx + 1, driver_name[m_carIdx].name().c_str(),
             m_bestLapTimeLapNum, bestLapSector1TimeInMS, bestLapSector1TimeInStr.c_str(), bestLapSector2TimeInMS,
             bestLapSector2TimeInStr.c_str(), bestLapSector3TimeInMS, bestLapSector3TimeInStr.c_str(), bestLapTimeInMS,
             bestLapTimeInStr.c_str(), m_bestSector1LapNum, bestSector1TimeInMS, bestSector1TimeInStr.c_str(),
             m_bestSector2LapNum, bestSector2TimeInMS, bestSector2TimeInStr.c_str(), m_bestSector3LapNum,
             bestSector3TimeInMS, bestSector3TimeInStr.c_str(), theoreticalBestLapTimeInMS,
             theoreticalBestLapTimeInStr.c_str());

    sql += stmt;

    return sql;
  }
};

#pragma pack(pop)