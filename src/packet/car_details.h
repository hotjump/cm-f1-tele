#pragma once

#include <math.h>
#include "packet.h"
#include "packet_header.h"
#include "packet/participants.h"
#include "packet/car_motion.h"
#include "packet/car_telemetry.h"
#include "packet/lap_data.h"

/*
Packets details refers to car_motion.h and car_telemetry.h and lap_data.h
*/


struct CarDetails {
  static std::string ToSQL(FuntionCommonArg, ParticipantDataArg, const PacketMotionData& car_motion,
    const PacketCarTelemetryData& car_telemetry, const PacketLapData& lap_data) {
    std::string sql;
    char stmt[512] = {0};
    sql += "INSERT INTO CarDetails Values\n";
    const char* fmt = 
        "(%u,%u,%u,%u,NOW(),%u,'%s',%u,%.2f,%.2f,%.2f,%u,%i,%u,%u,%.2f,%.2f,%.2f,%u,'%s',%u,'%s',%u,'%s',%.3f,%.3f,%u,%u),\n";
    
    auto p_motion = car_motion.m_carMotionData;
    auto p_tele = car_telemetry.m_carTelemetryData;
    auto p_lap = lap_data.m_lapData;

    bool tt_mode = false;
    if (lap_data.m_timeTrialPBCarIdx != 255 || lap_data.m_timeTrialRivalCarIdx != 255) {
      tt_mode = true;
    }
    
    for (uint8 i = 0; i < dirver_num; i++) {
      if (!tt_mode || (tt_mode && (i == 0 || i == lap_data.m_timeTrialPBCarIdx || i == lap_data.m_timeTrialRivalCarIdx))) {
        ;
      } else {
        continue;
      }
      TimeFormat currentLapTimeInMS(p_lap[i].m_currentLapTimeInMS);
      TimeFormat sector1TimeInMS(p_lap[i].m_sector1TimeInMS);
      TimeFormat sector2TimeInMS(p_lap[i].m_sector2TimeInMS);
      snprintf(stmt, sizeof(stmt) ,fmt, PrimaryKeyCommonPart, lap_data.m_header.m_frameIdentifier, i + 1, driver_name[i].name().c_str(), p_tele[i].m_speed,
              p_tele[i].m_throttle, p_tele[i].m_steer, p_tele[i].m_brake, p_tele[i].m_clutch, p_tele[i].m_gear, p_tele[i].m_engineRPM, p_tele[i].m_drs,
              p_motion[i].m_worldPositionX, p_motion[i].m_worldPositionY, p_motion[i].m_worldPositionZ,
              p_lap[i].m_currentLapTimeInMS, currentLapTimeInMS.c_str(), p_lap[i].m_sector1TimeInMS, sector1TimeInMS.c_str(),
              p_lap[i].m_sector2TimeInMS, sector2TimeInMS.c_str(), p_lap[i].m_lapDistance, p_lap[i].m_totalDistance, p_lap[i].m_carPosition, p_lap[i].m_currentLapNum);
      sql += stmt;
    }

    sql[sql.size() - 2] = ';';

    return sql;
  }
};
