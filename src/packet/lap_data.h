#pragma once

#include "packet_header.h"

/*
The lap data packet gives details of all the cars in the session.

Frequency : Rate as specified in menus Size : 972 bytes Version : 1
*/

ENUM(PitStatus, none, pitting, in_pit_area);

ENUM(DriversStatus, in_garage, flying_lap, in_lap, out_lap, on_track);

#pragma pack(push, 1)

struct LapData {
  uint32 m_lastLapTimeInMS;             // Last lap time in milliseconds
  uint32 m_currentLapTimeInMS;          // Current time around the lap in milliseconds
  uint16 m_sector1TimeInMS;             // Sector 1 time in milliseconds
  uint16 m_sector2TimeInMS;             // Sector 2 time in milliseconds
  float m_lapDistance;                  // Distance vehicle is around current lap in metres – could
                                        // be negative if line hasn’t been crossed yet
  float m_totalDistance;                // Total distance travelled in session in metres – could
                                        // be negative if line hasn’t been crossed yet
  float m_safetyCarDelta;               // Delta in seconds for safety car
  uint8 m_carPosition;                  // Car race position
  uint8 m_currentLapNum;                // Current lap number
  uint8 m_pitStatus;                    // 0 = none, 1 = pitting, 2 = in pit area
  uint8 m_numPitStops;                  // Number of pit stops taken in this race
  uint8 m_sector;                       // 0 = sector1, 1 = sector2, 2 = sector3
  uint8 m_currentLapInvalid;            // Current lap invalid - 0 = valid, 1 = invalid
  uint8 m_penalties;                    // Accumulated time penalties in seconds to be added
  uint8 m_warnings;                     // Accumulated number of warnings issued
  uint8 m_numUnservedDriveThroughPens;  // Num drive through pens left to serve
  uint8 m_numUnservedStopGoPens;        // Num stop go pens left to serve
  uint8 m_gridPosition;                 // Grid position the vehicle started the race in
  uint8 m_driverStatus;                 // Status of driver - 0 = in garage, 1 = flying lap
                                        // 2 = in lap, 3 = out lap, 4 = on track
  uint8 m_resultStatus;                 // Result status - 0 = invalid, 1 = inactive, 2 = active
                                        // 3 = finished, 4 = didnotfinish, 5 = disqualified
                                        // 6 = not classified, 7 = retired
  uint8 m_pitLaneTimerActive;           // Pit lane timing, 0 = inactive, 1 = active
  uint16 m_pitLaneTimeInLaneInMS;       // If active, the current time spent in the pit lane in ms
  uint16 m_pitStopTimerInMS;            // Time of the actual pit stop in ms
  uint8 m_pitStopShouldServePen;        // Whether the car should serve a penalty at this stop
};

struct PacketLapData {
  PacketHeader m_header;  // Header

  LapData m_lapData[22];  // Lap data for all cars on track

  uint8 m_timeTrialPBCarIdx;     // Index of Personal Best car in time trial (255 if invalid)
  uint8 m_timeTrialRivalCarIdx;  // Index of Rival car in time trial (255 if invalid)

  std::string ToSQL(uint32_t begin, uint32_t current, uint8 dirver_num, const ParticipantData* driver_name) const {
    std::string sql;
    sql.reserve(4 * 1024);
    char stmt[512] = {0};
    const LapData* p = m_lapData;
    const char* fmt =
        "INSERT INTO LapData "
        "Values(%u,%u,NOW(),%u,'%s','%s',%u,'%s',%u,'%s',%u,'%s',%u,'%s',%.3f,%.3f,%.3f,%u,%u,%u,'%s',%u,%u,%u,%u,%u,%"
        "u,%u,%u,%u,'%s',%"
        "u,'%s',%u,%u,%u,%u);\n";

    int tt_mode = false;
    if (m_timeTrialPBCarIdx != 255 || m_timeTrialRivalCarIdx != 255) {
      tt_mode = true;
    }

    for (uint8 i = 0; i < dirver_num; i++) {
      TimeFormat lastLapTimeInMS(p[i].m_lastLapTimeInMS);
      TimeFormat currentLapTimeInMS(p[i].m_currentLapTimeInMS);
      TimeFormat sector1TimeInMS(p[i].m_sector1TimeInMS);
      TimeFormat sector2TimeInMS(p[i].m_sector2TimeInMS);

      if (!tt_mode || (tt_mode && (i == 0 || i == m_timeTrialPBCarIdx || i == m_timeTrialRivalCarIdx))) {
        ;
      } else {
        continue;
      }

      snprintf(stmt, sizeof(stmt), fmt, begin, current, i + 1, driver_name[i].m_name,
               (driver_name[i].m_teamId == 255 ? "-" : TeamName.at(driver_name[i].m_teamId)), p[i].m_lastLapTimeInMS,
               lastLapTimeInMS.c_str(), p[i].m_currentLapTimeInMS, currentLapTimeInMS.c_str(), p[i].m_sector1TimeInMS,
               sector1TimeInMS.c_str(), p[i].m_sector2TimeInMS, sector2TimeInMS.c_str(), p[i].m_lapDistance,
               p[i].m_totalDistance, p[i].m_safetyCarDelta, p[i].m_carPosition, p[i].m_currentLapNum, p[i].m_pitStatus,
               EnumToCStr(PitStatus, p[i].m_pitStatus), p[i].m_numPitStops, p[i].m_sector, p[i].m_currentLapInvalid,
               p[i].m_penalties, p[i].m_warnings, p[i].m_numUnservedDriveThroughPens, p[i].m_numUnservedStopGoPens,
               p[i].m_gridPosition, p[i].m_driverStatus, EnumToCStr(DriversStatus, p[i].m_driverStatus),
               p[i].m_resultStatus, EnumToCStr(ResultStatus, p[i].m_resultStatus), p[i].m_pitLaneTimerActive,
               p[i].m_pitLaneTimeInLaneInMS, p[i].m_pitStopTimerInMS, p[i].m_pitStopShouldServePen);
      sql += stmt;
    }
    return sql;
  }
};

#pragma pack(pop)