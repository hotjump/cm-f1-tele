#pragma once

#include <stdio.h>
#include <stdlib.h>

#include "car_status.h"
#include "common/time_format.h"
#include "packet_header.h"
#include "session.h"

/*
This packet details the final classification at the end of the race, and the data will match with the post race results
screen. This is especially useful for multiplayer games where it is not always possible to send lap times on the final
frame because of network delay.

Frequency: Once at the end of a race
Size: 1015 bytes
Version: 1
*/

#pragma pack(push, 1)

struct FinalClassificationData {
  uint8 m_position;              // Finishing position
  uint8 m_numLaps;               // Number of laps completed
  uint8 m_gridPosition;          // Grid position of the car
  uint8 m_points;                // Number of points scored
  uint8 m_numPitStops;           // Number of pit stops made
  uint8 m_resultStatus;          // Result status - 0 = invalid, 1 = inactive, 2 = active
                                 // 3 = finished, 4 = didnotfinish, 5 = disqualified
                                 // 6 = not classified, 7 = retired
  uint32 m_bestLapTimeInMS;      // Best lap time of the session in milliseconds
  double m_totalRaceTime;        // Total race time in seconds without penalties
  uint8 m_penaltiesTime;         // Total penalties accumulated in seconds
  uint8 m_numPenalties;          // Number of penalties applied to this driver
  uint8 m_numTyreStints;         // Number of tyres stints up to maximum
  uint8 m_tyreStintsActual[8];   // Actual tyres used by this driver
  uint8 m_tyreStintsVisual[8];   // Visual tyres used by this driver
  uint8 m_tyreStintsEndLaps[8];  // The lap number stints end on
};

struct PacketFinalClassificationData {
  PacketHeader m_header;  // Header

  uint8 m_numCars;  // Number of cars in the final classification
  FinalClassificationData m_classificationData[22];

  std::string ToOverallResult(bool is_race, const FinalClassificationData* p,
                              const FinalClassificationData* p_first) const {
    // 如果是第一名，直接写成绩，
    // 1. 排位赛写GAP/NO TIME等其他信息
    // 2. 正赛写GAP/DNF/+ xlaps
    std::string overall_result;
    if (is_race) {
      if (p->m_position == 1) {
        TimeFormat totalRaceTimeStr(p->m_totalRaceTime);
        overall_result = totalRaceTimeStr.c_str();
      } else if (static_cast<ResultStatus>(p->m_resultStatus) == ResultStatus::finished) {
        auto lap_gap = p_first->m_numLaps - p->m_numLaps;
        if (lap_gap == 0) {
          TimeFormat gap_to_leader(p->m_totalRaceTime + p->m_penaltiesTime - p_first->m_totalRaceTime);
          overall_result = std::string("+") + gap_to_leader.c_str() + std::string("s");
        } else {
          overall_result = (lap_gap == 1 ? "+1 lap" : "+" + std::to_string(lap_gap) + " laps");
        }
      } else {
        overall_result = "DNF";
      }
    } else {
      if (p->m_position == 1) {
        TimeFormat bestLapTimeStr(p->m_bestLapTimeInMS);
        overall_result = bestLapTimeStr.c_str();
      } else if (p->m_bestLapTimeInMS > 0) {
        TimeFormat gap_to_leader(p->m_bestLapTimeInMS - p_first->m_bestLapTimeInMS);
        overall_result = std::string("+") + gap_to_leader.c_str() + std::string("s");
      } else {
        overall_result = "NO TIME";
      }
    }
    return overall_result;
  }

  std::string ToSQL(FuntionCommonArg, ParticipantDataArg, const PacketSessionData* session) const {
    bool is_race = session->IsRace();

    std::string sql;
    sql.reserve(4 * 1024);
    const char* fmt =
        "REPLACE INTO FinalClassification "
        "Values(%u,%u,%u,NOW(),%u,'%s','%s',%u,%u,%u,%u,%u,%u,'%s',%u,'%s',%.3f,'%s',%u,%u,%u,'%s','%s');\n";
    char stmt[512] = {0};
    const FinalClassificationData* p = m_classificationData;
    const FinalClassificationData* p_first = nullptr;

    // 找到第一名
    for (uint8 i = 0; i < m_numCars; i++) {
      if (p[i].m_position == 1) {
        p_first = &p[i];
        break;
      }
    }

    for (uint8 i = 0; i < m_numCars; i++) {
      if (static_cast<ResultStatus>(p[i].m_resultStatus) == ResultStatus::not_classified) {
        continue;
      }

      TimeFormat bestLapTimeStr(p[i].m_bestLapTimeInMS);
      TimeFormat totalRaceTimeStr(p[i].m_totalRaceTime);

      std::string tyre_stint;
      char buffer[32] = {0};

      for (int ii = 0; ii < p[i].m_numTyreStints; ii++) {
        tyre_stint += std::string("| ") + (EnumToCStr(VisualTyreCompound, p[i].m_tyreStintsVisual[ii]));
        auto end_lap = p[i].m_tyreStintsEndLaps[ii];
        snprintf(buffer, sizeof(buffer), " (%u) ", end_lap);
        tyre_stint += buffer;
      }

      tyre_stint += "|";

      std::string overall_result = ToOverallResult(is_race, &p[i], p_first);

      snprintf(stmt, sizeof(stmt), fmt, PrimaryKeyCommonPart, i + 1, driver_name[i].name().c_str(),
               (driver_name[i].m_teamId == 255 ? "-" : TeamName.at(driver_name[i].m_teamId)), p[i].m_position,
               p[i].m_numLaps, p[i].m_gridPosition, p[i].m_points, p[i].m_numPitStops, p[i].m_resultStatus,
               EnumToCStr(ResultStatus, p[i].m_resultStatus), p[i].m_bestLapTimeInMS, bestLapTimeStr.c_str(),
               p[i].m_totalRaceTime, totalRaceTimeStr.c_str(), p[i].m_penaltiesTime, p[i].m_numPenalties,
               p[i].m_numTyreStints, tyre_stint.c_str(), overall_result.c_str());
      sql += stmt;
    }

    return sql;
  }
};

#pragma pack(pop)
