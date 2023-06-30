#pragma once

#include <string.h>

#include "packet_header.h"

/*
  The session packet includes details about the current session in progress.
  Frequency : 2 per second
  Size : 632 bytes
  Version : 1
*/

ENUM(ZoneFlag, NONE, GREEN, BLUE, YELLOW, RED);

ENUM(TemperatureChange, UP, DOWN, NO_CHANGE);

ENUM(Formula, F1, F1_CLASSIC, F2, F1_GENERIC, BETA, SUPERCARS, ESPORTS, F1_2011);

ENUM(SafetyCarStatus, NO_SATETY_CAR, FULL, VIRTUAL, FORMATION_LAP);

ENUM(GameMode, Event_Mode, None1, None2, Grand_Prix, None4, Time_Trial, Splitscreen, Online_Custom, Online_League,
     None9, None10, Career_Invitational, Championship_Invitational, Online_Championship, Online_Weekly_Event, None16,
     None17, None18, Career_22, Career_22_Online);

ENUM(Ruleset, Practice_Qualifying, Race, Time_Trial, None1, Time_Attack, None2, Checkpoint_Challenge, None3, Autocross,
     Drift, Average_Speed_Zone, Rival_Duel);

ENUM(SessionType, UNKNOWN, P1, P2, P3, SHORT_P, Q1, Q2, Q3, SHORT_Q, OSQ, R, R2, R3, TIME_TRIAL);

ENUM(Weather, CLEAR, LIGHT_CLOUD, OVERCAST, LIGHT_RAIN, HEAVY_RAIN, STORM);

ENUM(SessionLength, None, Invalid, Very_Short, Short, Medium, Medium_Long, Long, Full);

#pragma pack(push, 1)

struct MarshalZone {
  float m_zoneStart;  // Fraction (0..1) of way through the lap the marshal zone starts
  int8 m_zoneFlag;    // -1 = invalid/unknown, 0 = none, 1 = green, 2 = blue, 3 = yellow, 4 = red
};

struct WeatherForecastSample {
  uint8 m_sessionType;            // 0 = unknown, 1 = P1, 2 = P2, 3 = P3, 4 = Short P, 5 = Q1
                                  // 6 = Q2, 7 = Q3, 8 = Short Q, 9 = OSQ, 10 = R, 11 = R2
                                  // 12 = R3, 13 = Time Trial
  uint8 m_timeOffset;             // Time in minutes the forecast is for
  uint8 m_weather;                // Weather - 0 = clear, 1 = light cloud, 2 = overcast
                                  // 3 = light rain, 4 = heavy rain, 5 = storm
  int8 m_trackTemperature;        // Track temp. in degrees Celsius
  int8 m_trackTemperatureChange;  // Track temp. change – 0 = up, 1 = down, 2 = no change
  int8 m_airTemperature;          // Air temp. in degrees celsius
  int8 m_airTemperatureChange;    // Air temp. change – 0 = up, 1 = down, 2 = no change
  uint8 m_rainPercentage;         // Rain percentage (0-100)
};

struct PacketSessionData {
  PacketSessionData() { memset(this, 0, sizeof(PacketSessionData)); }
  PacketHeader m_header;  // Header

  uint8 m_weather;                                     // Weather - 0 = clear, 1 = light cloud, 2 = overcast
                                                       // 3 = light rain, 4 = heavy rain, 5 = storm
  int8 m_trackTemperature;                             // Track temp. in degrees celsius
  int8 m_airTemperature;                               // Air temp. in degrees celsius
  uint8 m_totalLaps;                                   // Total number of laps in this race
  uint16 m_trackLength;                                // Track length in metres
  uint8 m_sessionType;                                 // 0 = unknown, 1 = P1, 2 = P2, 3 = P3, 4 = Short P
                                                       // 5 = Q1, 6 = Q2, 7 = Q3, 8 = Short Q, 9 = OSQ
                                                       // 10 = R, 11 = R2, 12 = R3, 13 = Time Trial
  int8 m_trackId;                                      // -1 for unknown, see appendix
  uint8 m_formula;                                     // Formula, 0 = F1 Modern, 1 = F1 Classic, 2 = F2,
                                                       // 3 = F1 Generic, 4 = Beta, 5 = Supercars
                                                       // 6 = Esports, 7 = F2 2021
  uint16 m_sessionTimeLeft;                            // Time left in session in seconds
  uint16 m_sessionDuration;                            // Session duration in seconds
  uint8 m_pitSpeedLimit;                               // Pit speed limit in kilometres per hour
  uint8 m_gamePaused;                                  // Whether the game is paused – network game only
  uint8 m_isSpectating;                                // Whether the player is spectating
  uint8 m_spectatorCarIndex;                           // Index of the car being spectated
  uint8 m_sliProNativeSupport;                         // SLI Pro support, 0 = inactive, 1 = active
  uint8 m_numMarshalZones;                             // Number of marshal zones to follow
  MarshalZone m_marshalZones[21];                      // List of marshal zones – max 21
  uint8 m_safetyCarStatus;                             // 0 = no safety car, 1 = full
                                                       // 2 = virtual, 3 = formation lap
  uint8 m_networkGame;                                 // 0 = offline, 1 = online
  uint8 m_numWeatherForecastSamples;                   // Number of weather samples to follow
  WeatherForecastSample m_weatherForecastSamples[56];  // Array of weather forecast samples
  uint8 m_forecastAccuracy;                            // 0 = Perfect, 1 = Approximate
  uint8 m_aiDifficulty;                                // AI Difficulty rating – 0-110
  uint32 m_seasonLinkIdentifier;                       // Identifier for season - persists across saves
  uint32 m_weekendLinkIdentifier;                      // Identifier for weekend - persists across saves
  uint32 m_sessionLinkIdentifier;                      // Identifier for session - persists across saves
  uint8 m_pitStopWindowIdealLap;                       // Ideal lap to pit on for current strategy (player)
  uint8 m_pitStopWindowLatestLap;                      // Latest lap to pit on for current strategy (player)
  uint8 m_pitStopRejoinPosition;                       // Predicted position to rejoin at (player)
  uint8 m_steeringAssist;                              // 0 = off, 1 = on
  uint8 m_brakingAssist;                               // 0 = off, 1 = low, 2 = medium, 3 = high
  uint8 m_gearboxAssist;                               // 1 = manual, 2 = manual & suggested gear, 3 = auto
  uint8 m_pitAssist;                                   // 0 = off, 1 = on
  uint8 m_pitReleaseAssist;                            // 0 = off, 1 = on
  uint8 m_ERSAssist;                                   // 0 = off, 1 = on
  uint8 m_DRSAssist;                                   // 0 = off, 1 = on
  uint8 m_dynamicRacingLine;                           // 0 = off, 1 = corners only, 2 = full
  uint8 m_dynamicRacingLineType;                       // 0 = 2D, 1 = 3D
  uint8 m_gameMode;                                    // Game mode id - see appendix
  uint8 m_ruleSet;                                     // Ruleset - see appendix
  uint32 m_timeOfDay;                                  // Local time of day - minutes since midnight
  uint8 m_sessionLength;                               // 0 = None, 2 = Very Short, 3 = Short, 4 = Medium
  // 5 = Medium Long, 6 = Long, 7 = Full

  bool IsRace() const {
    switch (static_cast<SessionType>(m_sessionType)) {
      case SessionType::R:
      case SessionType::R2:
      case SessionType::R3: {
        return true;
      } break;
      case SessionType::P1:
      case SessionType::P2:
      case SessionType::P3:
      case SessionType::SHORT_P:
      case SessionType::Q1:
      case SessionType::Q2:
      case SessionType::Q3:
      case SessionType::SHORT_Q:
      case SessionType::OSQ:
      case SessionType::TIME_TRIAL: {
        return false;
      } break;
      default:
        break;
    }
    return false;
  }

  bool IsTTMode() const {
    return static_cast<SessionType>(m_sessionType) == SessionType::TIME_TRIAL;
  }

  bool HaveRealUserName() const {
    if (m_networkGame > 0) {
      return true;
    }
    switch (static_cast<GameMode>(m_gameMode)) {
      case GameMode::Time_Trial: {
        return true;
      } break;
      default: {
        return false;
      } break;
    }
    return false;
  }

  std::string ToSQL(FuntionCommonArg) const {
    char stmt[512] = {0};
    std::string sql;
    const char* fmt_session_data =
        "INSERT INTO SessionData "
        "Values(%u,%u,%u,NOW(),%u,'%s',%i,%i,%u,%u,%u,'%s',%i,'%s',%u,'%s',%u,%u,%u,%u,%u,%u,%u,%u,%u,'%s',%u,%u,%u,%u,"
        "%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,'%s',%u,'%s',%u,%u,'%s');\n";

    snprintf(stmt, sizeof(stmt), fmt_session_data, PrimaryKeyCommonPart, m_weather, EnumToCStr(Weather, m_weather),
             m_trackTemperature, m_airTemperature, m_totalLaps, m_trackLength, m_sessionType,
             EnumToCStr(SessionType, m_sessionType), m_trackId, EnumToCStr(Track, m_trackId), m_formula,
             EnumToCStr(Formula, m_formula), m_sessionTimeLeft, m_sessionDuration, m_pitSpeedLimit, m_gamePaused,
             m_isSpectating, m_spectatorCarIndex, m_sliProNativeSupport, m_numMarshalZones, m_safetyCarStatus,
             EnumToCStr(SafetyCarStatus, m_safetyCarStatus), m_networkGame, m_numWeatherForecastSamples,
             m_forecastAccuracy, m_aiDifficulty, m_seasonLinkIdentifier, m_weekendLinkIdentifier,
             m_sessionLinkIdentifier, m_pitStopWindowIdealLap, m_pitStopWindowLatestLap, m_pitStopRejoinPosition,
             m_steeringAssist, m_brakingAssist, m_gearboxAssist, m_pitAssist, m_pitReleaseAssist, m_ERSAssist,
             m_DRSAssist, m_dynamicRacingLine, m_dynamicRacingLineType, m_gameMode, EnumToCStr(GameMode, m_gameMode),
             m_ruleSet, EnumToCStr(Ruleset, m_ruleSet), m_timeOfDay, m_sessionLength,
             EnumToCStr(SessionLength, m_sessionLength));

    sql += stmt;

    if (m_numWeatherForecastSamples > 0) {
      sql += "INSERT INTO WeatherForecast Values\n";
    }

    const char* fmt_weather_forcast = "(%u,%u,%u,NOW(),%u,%u,'%s',%u,'%s',%i,%i,'%s',%i,%i,'%s',%u)%c\n";
    const WeatherForecastSample* p_w = m_weatherForecastSamples;

    for (uint8 i = 0; i < m_numWeatherForecastSamples; i++) {
      snprintf(stmt, sizeof(stmt), fmt_weather_forcast, PrimaryKeyCommonPart, p_w[i].m_timeOffset, p_w[i].m_sessionType,
               EnumToCStr(SessionType, p_w[i].m_sessionType), p_w[i].m_weather, EnumToCStr(Weather, p_w[i].m_weather),
               p_w[i].m_trackTemperature, p_w[i].m_trackTemperatureChange,
               EnumToCStr(TemperatureChange, p_w[i].m_trackTemperatureChange), p_w[i].m_airTemperature,
               p_w[i].m_airTemperatureChange, EnumToCStr(TemperatureChange, p_w[i].m_airTemperatureChange),
               p_w[i].m_rainPercentage, i + 1 == m_numWeatherForecastSamples ? ';' : ',');
      sql += stmt;
    }

    sql += "INSERT INTO MarshalZones Values\n";

    const char* fmt_marshal = "(%u,%u,%u,NOW(),%u,%.2f,%i,'%s')%c\n";

    const MarshalZone* p_m = m_marshalZones;

    for (uint8 i = 0; i < m_numMarshalZones; i++) {
      snprintf(stmt, sizeof(stmt), fmt_marshal, PrimaryKeyCommonPart, i + 1, p_m[i].m_zoneStart, p_m[i].m_zoneFlag,
               EnumToCStr(ZoneFlag, p_m[i].m_zoneFlag), i + 1 == m_numMarshalZones ? ';' : ',');
      sql += stmt;
    }

    return sql;
  }
};

#pragma pack(pop)
