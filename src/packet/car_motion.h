#pragma once

#include <math.h>

#include "packet.h"
#include "packet/participants.h"
#include "packet_header.h"

/*
The motion packet gives physics data for all the cars being driven. There is additional data for the car being driven
with the goal of being able to drive a motion platform setup. N.B. For the normalised vectors below, to convert to float
values divide by 32767.0f – 16-bit signed values are used to pack the data and on the assumption that direction values
are always between -1.0f and 1.0f.

Frequency: Rate as specified in menus
Size: 1464 bytes
Version: 1
*/
#pragma pack(push, 1)

struct CarMotionData {
  float m_worldPositionX;      // World space X position
  float m_worldPositionY;      // World space Y position
  float m_worldPositionZ;      // World space Z position
  float m_worldVelocityX;      // Velocity in world space X
  float m_worldVelocityY;      // Velocity in world space Y
  float m_worldVelocityZ;      // Velocity in world space Z
  int16 m_worldForwardDirX;    // World space forward X direction (normalised)
  int16 m_worldForwardDirY;    // World space forward Y direction (normalised)
  int16 m_worldForwardDirZ;    // World space forward Z direction (normalised)
  int16 m_worldRightDirX;      // World space right X direction (normalised)
  int16 m_worldRightDirY;      // World space right Y direction (normalised)
  int16 m_worldRightDirZ;      // World space right Z direction (normalised)
  float m_gForceLateral;       // Lateral G-Force component
  float m_gForceLongitudinal;  // Longitudinal G-Force component
  float m_gForceVertical;      // Vertical G-Force component
  float m_yaw;                 // Yaw angle in radians
  float m_pitch;               // Pitch angle in radians
  float m_roll;                // Roll angle in radians
};

struct PacketMotionData {
  PacketHeader m_header;  // Header

  CarMotionData m_carMotionData[22];  // Data for all cars on track

  // Extra player car ONLY data
  float m_suspensionPosition[4];      // Note: All wheel arrays have the following order:
  float m_suspensionVelocity[4];      // RL, RR, FL, FR
  float m_suspensionAcceleration[4];  // RL, RR, FL, FR
  float m_wheelSpeed[4];              // Speed of each wheel
  float m_wheelSlip[4];               // Slip ratio for each wheel
  float m_localVelocityX;             // Velocity in local space
  float m_localVelocityY;             // Velocity in local space
  float m_localVelocityZ;             // Velocity in local space
  float m_angularVelocityX;           // Angular velocity x-component
  float m_angularVelocityY;           // Angular velocity y-component
  float m_angularVelocityZ;           // Angular velocity z-component
  float m_angularAccelerationX;       // Angular velocity x-component
  float m_angularAccelerationY;       // Angular velocity y-component
  float m_angularAccelerationZ;       // Angular velocity z-component
  float m_frontWheelsAngle;           // Current front wheels angle in radians

  void coordinate_rotation(float degree, float in_x, float in_z, float& out_x, float& out_z) const {
    float beta = degree / 180 * 3.1415926;
    out_x = in_x * cos(beta) - in_z * sin(beta);
    out_z = in_x * sin(beta) + in_z * cos(beta);
  }

  std::string ToSQL(uint32_t begin, uint32_t current, uint8 dirver_num, const ParticipantData* driver_name,
                    int track_id) const {
    std::string sql;
    sql.reserve(4 * 1024);

    float lat = 24.471000;
    float lon = 54.605650;
    float degree = 18.5;
    int div_x = 108000;
    int div_y = 100000;

    switch (static_cast<Track>(track_id)) {
      case Track::Mexico: {
        lat = 19.3957984;
        lon = -99.08745;
        degree = 90;
        div_x = 100000;
        div_y = 100000;
        break;
      }
      case Track::Hungaroring: {
        lat = 47.583397704;
        lon = 19.24989903;
        degree = 80;
        div_x = 95000;
        div_y = 90000;
        break;
      }
      case Track::Monaco: {
        lat = 43.73633;
        lon = 7.4230;
        degree = 110;
        div_x = 100000;
        div_y = 100000;
        break;
      }
      case Track::Melbourne: {
        lat = -37.846167;
        lon = 144.9709;
        degree = 93;
        div_x = 110000;
        div_y = 90000;
        break;
      }
      case Track::Catalunya: {
        lat = 41.56922;
        lon = 2.2581448;
        degree = 93;
        div_x = 105500;
        div_y = 90000;
        break;
      }
      case Track::Zandvoort: {
        lat = 52.388021;
        lon = 4.545618;
        degree = 93;
        div_x = 110000;
        div_y = 70000;
        break;
      }
      case Track::Shanghai: {
        lat = 31.34070;
        lon = 121.22126;
        degree = 87;
        div_x = 107000;
        div_y = 100000;
        break;
      }
      case Track::Hanoi: {
        lat = 21.017228;
        lon = 105.765524;
        degree = 89;
        div_x = 111000;
        div_y = 100000;
        break;
      }
      case Track::Paul_Ricard: {
        lat = 43.252324;
        lon = 5.791019;
        degree = 88;
        div_x = 98000;
        div_y = 82000;
        break;
      }
      case Track::Sakhir: {
        lat = 26.0315;
        lon = 50.514255;
        degree = 89;
        div_x = 108000;
        div_y = 100000;
        break;
      }
      case Track::Spa: {
        lat = 50.43650;
        lon = 5.970100;
        degree = 93;
        div_x = 111100;
        div_y = 70500;
        break;
      }
      case Track::Baku: {
        lat = 40.3676;
        lon = 49.8446;
        degree = 92;
        div_x = 100000;
        div_y = 85000;
        break;
      }
      case Track::Texas: {
        lat = 30.139599;
        lon = -97.634600;
        degree = 92;
        div_x = 103000;
        div_y = 103000;
        break;
      }
      case Track::Suzuka: {
        lat = 34.84336;
        lon = 136.53288;
        degree = 88;
        div_x = 100000;
        div_y = 100000;
        break;
      }
      case Track::Austria: {
        lat = 47.222023;
        lon = 14.761222;
        degree = 90;
        div_x = 110000;
        div_y = 75000;
        break;
      }
      case Track::Sochi: {
        lat = 43.407634;
        lon = 39.958545;
        degree = 92;
        div_x = 110000;
        div_y = 81500;
        break;
      }
      case Track::Silverstone: {
        lat = 52.072021;
        lon = -1.015069;
        degree = 92;
        div_x = 110000;
        div_y = 70000;
        break;
      }
      case Track::Monza: {
        lat = 45.621561;
        lon = 9.288896;
        degree = 90;
        div_x = 110000;
        div_y = 77000;
        break;
      }
      case Track::Montreal: {
        lat = 45.510285;
        lon = -73.527499;
        degree = 90;
        div_x = 111000;
        div_y = 77000;
        break;
      }
      case Track::Brazil: {
        lat = -23.70111;
        lon = -46.693224;
        degree = 90;
        div_x = 111000;
        div_y = 77000;
        break;
      }

      default:
        break;
    }

    const char* fmt =
        "INSERT INTO CarMotion "
        "Values(%u,%u,NOW(),%u,'%s',%f,%f,%f,%f,%f,%f,%i,%i,%i,%i,%i,%i,%f,%f,%f,%f,%f,%f,%f,%f);\n";
    char stmt[512] = {0};
    const CarMotionData* p = m_carMotionData;

    for (uint8 i = 0; i < dirver_num; i++) {
      float out_x;
      float out_z;

      if (p[i].m_worldPositionX == 0 && p[i].m_worldPositionY == 0 && p[i].m_worldPositionZ == 0) continue;

      coordinate_rotation(degree, p[i].m_worldPositionX, p[i].m_worldPositionZ, out_x, out_z);

      snprintf(stmt, sizeof(stmt), fmt, begin, current, i + 1, driver_name[i].name().c_str(), p[i].m_worldPositionX,
               p[i].m_worldPositionY, p[i].m_worldPositionZ, p[i].m_worldVelocityX, p[i].m_worldVelocityY,
               p[i].m_worldVelocityZ, p[i].m_worldForwardDirX, p[i].m_worldForwardDirY, p[i].m_worldForwardDirZ,
               p[i].m_worldRightDirX, p[i].m_worldRightDirY, p[i].m_worldRightDirZ, p[i].m_gForceLateral,
               p[i].m_gForceLongitudinal, p[i].m_gForceVertical, p[i].m_yaw, p[i].m_pitch, p[i].m_roll,
               out_x / div_x + lat, out_z / div_y + lon);

      sql += stmt;
    }
    return sql;
  }
};

#pragma pack(pop)

// constexpr uint16 MotionDataPacketSize = sizeof(CarMotionData);
