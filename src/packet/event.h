#pragma once

#include <iostream>

#include "common/log.h"
#include "packet_header.h"
#include "participants.h"
#include "string.h"

/*
This packet gives details of events that happen during the course of a session.

Frequency: When the event occurs
Size: 36 bytes
Version: 1

*/

// The event details packet is different for each type of event.
// Make sure only the correct type is interpreted.

const std::vector<const char*> penaltyTypeStr = {
    "Drive through",
    "Stop Go",
    "Grid penalty",
    "Penalty reminder",
    "Time penalty",
    "Warning",
    "Disqualified",
    "Removed from formation lap",
    "Parked too long timer",
    "Tyre regulations",
    "This lap invalidated",
    "This and next lap invalidated",
    "This lap invalidated without reason",
    "This and next lap invalidated without reason",
    "This and previous lap invalidated",
    "This and previous lap invalidated without reason",
    "Retired",
    "Black flag timer",
};

const std::vector<const char*> InfringementTypesStr = {
    "Blocking by slow driving",
    "Blocking by wrong way driving",
    "Reversing off the start line",
    "Big Collision",
    "Small Collision",
    "Collision failed to hand back position single",
    "Collision failed to hand back position multiple",
    "Corner cutting gained time",
    "Corner cutting overtake single",
    "Corner cutting overtake multiple",
    "Crossed pit exit lane",
    "Ignoring blue flags",
    "Ignoring yellow flags",
    "Ignoring drive through",
    "Too many drive throughs",
    "Drive through reminder serve within n laps",
    "Drive through reminder serve this lap",
    "Pit lane speeding",
    "Parked for too long",
    "Ignoring tyre regulations",
    "Too many penalties",
    "Multiple warnings",
    "Approaching disqualification",
    "Tyre regulations select single",
    "Tyre regulations select multiple",
    "Lap invalidated corner cutting",
    "Lap invalidated running wide",
    "Corner cutting ran wide gained time minor",
    "Corner cutting ran wide gained time significant",
    "Corner cutting ran wide gained time extreme",
    "Lap invalidated wall riding",
    "Lap invalidated flashback used",
    "Lap invalidated reset to track",
    "Blocking the pitlane",
    "Jump start",
    "Safety car to car collision",
    "Safety car illegal overtake",
    "Safety car exceeding allowed pace",
    "Virtual safety car exceeding allowed pace",
    "Formation lap below allowed speed",
    "Formation lap parking",
    "Retired mechanical failure",
    "Retired terminally damaged",
    "Safety car falling too far back",
    "Black flag timer",
    "Unserved stop go penalty",
    "Unserved drive through penalty",
    "Engine component change",
    "Gearbox change",
    "Parc Fermé change",
    "League grid penalty",
    "Retry penalty",
    "Illegal time gain",
    "Mandatory pitstop",
    "Attribute assigned",
};

#pragma pack(push, 1)

union EventDataDetails {
  struct {
    uint8 vehicleIdx;  // Vehicle index of car achieving fastest lap
    float lapTime;     // Lap time is in seconds
  } FastestLap;

  struct {
    uint8 vehicleIdx;  // Vehicle index of car retiring
  } Retirement;

  struct {
    uint8 vehicleIdx;  // Vehicle index of team mate
  } TeamMateInPits;

  struct {
    uint8 vehicleIdx;  // Vehicle index of the race winner
  } RaceWinner;

  struct {
    uint8 penaltyType;       // Penalty type – see Appendices
    uint8 infringementType;  // Infringement type – see Appendices
    uint8 vehicleIdx;        // Vehicle index of the car the penalty is applied to
    uint8 otherVehicleIdx;   // Vehicle index of the other car involved
    uint8 time;              // Time gained, or time spent doing action in seconds
    uint8 lapNum;            // Lap the penalty occurred on
    uint8 placesGained;      // Number of places gained by this
  } Penalty;

  struct {
    uint8 vehicleIdx;                  // Vehicle index of the vehicle triggering speed trap
    float speed;                       // Top speed achieved in kilometres per hour
    uint8 isOverallFastestInSession;   // Overall fastest speed in session = 1, otherwise 0
    uint8 isDriverFastestInSession;    // Fastest speed for driver in session = 1, otherwise 0
    uint8 fastestVehicleIdxInSession;  // Vehicle index of the vehicle that is the fastest
                                       // in this session
    float fastestSpeedInSession;       // Speed of the vehicle that is the fastest
                                       // in this session
  } SpeedTrap;

  struct {
    uint8 numLights;  // Number of lights showing
  } StartLIghts;

  struct {
    uint8 vehicleIdx;  // Vehicle index of the vehicle serving drive through
  } DriveThroughPenaltyServed;

  struct {
    uint8 vehicleIdx;  // Vehicle index of the vehicle serving stop go
  } StopGoPenaltyServed;

  struct {
    uint32 flashbackFrameIdentifier;  // Frame identifier flashed back to
    float flashbackSessionTime;       // Session time flashed back to
  } Flashback;

  struct {
    uint32 m_buttonStatus;  // Bit flags specifying which buttons are being pressed
                            // currently - see appendices
  } Buttons;
};

struct PacketEventData {
  PacketHeader m_header;  // Header

  char m_eventStringCode[4];        // Event string code, see below
  EventDataDetails m_eventDetails;  // Event details - should be interpreted differently
                                    // for each type

  std::string ToSQL(FuntionCommonArg, ParticipantDataArg, size_t i) const {
    std::string sql;
    char stmt[512] = {0};
    char desp[256] = {0};

    const char* fmt = "INSERT INTO EventUpdate Values(%u,%u,%u,now(),%u,'%s','%s','%s');\n";
    const char* fmt_pen = "INSERT INTO PenaltyUpdate Values(%u,%u,%u,now(),%u,'%s','%s','%s','%s','%s',%u,%u,%u);\n";

    if (strncmp(m_eventStringCode, "PENA", 4) == 0) {
      snprintf(stmt, sizeof(stmt), fmt_pen, PrimaryKeyCommonPart, i, "PENA",
               driver_name[m_eventDetails.Penalty.vehicleIdx].name().c_str(),
               m_eventDetails.Penalty.otherVehicleIdx == 255
                   ? "-"
                   : driver_name[m_eventDetails.Penalty.otherVehicleIdx].name().c_str(),
               ToCStr(penaltyTypeStr, m_eventDetails.Penalty.penaltyType, "unknown"),
               ToCStr(InfringementTypesStr, m_eventDetails.Penalty.infringementType, "unknown"),
               m_eventDetails.Penalty.time == 255 ? 0 : m_eventDetails.Penalty.time, m_eventDetails.Penalty.lapNum,
               m_eventDetails.Penalty.placesGained);
    } else if (strncmp(m_eventStringCode, "SSTA", 4) == 0) {
      snprintf(stmt, sizeof(stmt), fmt, PrimaryKeyCommonPart, i, "SSTA", "-", "The session starts!");
    } else if (strncmp(m_eventStringCode, "SEND", 4) == 0) {
      snprintf(stmt, sizeof(desp), fmt, PrimaryKeyCommonPart, i, "SEND", "-", "The session ends!");
    } else if (strncmp(m_eventStringCode, "FTLP", 4) == 0) {
      TimeFormat FastestLap(m_eventDetails.FastestLap.lapTime);
      snprintf(desp, sizeof(desp), "Fastest lap: %s, %s",
               driver_name[m_eventDetails.FastestLap.vehicleIdx].name().c_str(), FastestLap.c_str());
      snprintf(stmt, sizeof(stmt), fmt, PrimaryKeyCommonPart, i, "FTLP",
               driver_name[m_eventDetails.FastestLap.vehicleIdx].name().c_str(), desp);
    } else if (strncmp(m_eventStringCode, "RTMT", 4) == 0) {
      snprintf(desp, sizeof(desp), "Retired： %s", driver_name[m_eventDetails.Retirement.vehicleIdx].name().c_str());
      snprintf(stmt, sizeof(stmt), fmt, PrimaryKeyCommonPart, i, "RTMT",
               driver_name[m_eventDetails.Retirement.vehicleIdx].name().c_str(), desp);
    } else if (strncmp(m_eventStringCode, "DRSE", 4) == 0) {
      snprintf(stmt, sizeof(stmt), fmt, PrimaryKeyCommonPart, i, "DRSE", "-", "DRS enabled!");
    } else if (strncmp(m_eventStringCode, "DRSD", 4) == 0) {
      snprintf(stmt, sizeof(stmt), fmt, PrimaryKeyCommonPart, i, "DRSD", "-", "DRS disabled!");
    } else if (strncmp(m_eventStringCode, "TMPT", 4) == 0) {
      snprintf(stmt, sizeof(stmt), fmt, PrimaryKeyCommonPart, i, "TMPT", "-", "Your team mate has entered the pits!");
    } else if (strncmp(m_eventStringCode, "CHQF", 4) == 0) {
      snprintf(stmt, sizeof(stmt), fmt, PrimaryKeyCommonPart, i, "CHQF", "-", "The chequered flag has been waved!");
    } else if (strncmp(m_eventStringCode, "RCWN", 4) == 0) {
      snprintf(desp, sizeof(desp), "Winner: %s", driver_name[m_eventDetails.RaceWinner.vehicleIdx].name().c_str());
      snprintf(stmt, sizeof(stmt), fmt, PrimaryKeyCommonPart, i, "RCWN",
               driver_name[m_eventDetails.RaceWinner.vehicleIdx].name().c_str(), desp);
    } else if (strncmp(m_eventStringCode, "SPTP", 4) == 0) {
      snprintf(desp, sizeof(desp), "Speed Trap: %s, %.1fKM/H",
               driver_name[m_eventDetails.SpeedTrap.vehicleIdx].name().c_str(), m_eventDetails.SpeedTrap.speed);
      snprintf(stmt, sizeof(stmt), fmt, PrimaryKeyCommonPart, i, "SPTP",
               driver_name[m_eventDetails.SpeedTrap.vehicleIdx].name().c_str(), desp);
    } else if (strncmp(m_eventStringCode, "STLG", 4) == 0) {
      snprintf(desp, sizeof(desp), "StarLight: %u", m_eventDetails.StartLIghts.numLights);
      snprintf(stmt, sizeof(stmt), fmt, PrimaryKeyCommonPart, i, "STLG", "-", desp);
    } else if (strncmp(m_eventStringCode, "LGOT", 4) == 0) {
      snprintf(stmt, sizeof(stmt), fmt, PrimaryKeyCommonPart, i, "LGOT", "-", "Lights out!");
    } else if (strncmp(m_eventStringCode, "DTSV", 4) == 0) {
      snprintf(desp, sizeof(desp), "Drive through penalty served: %s",
               driver_name[m_eventDetails.DriveThroughPenaltyServed.vehicleIdx].name().c_str());
      snprintf(stmt, sizeof(stmt), fmt, PrimaryKeyCommonPart, i, "DTSV",
               driver_name[m_eventDetails.DriveThroughPenaltyServed.vehicleIdx].name().c_str(), desp);
    } else if (strncmp(m_eventStringCode, "SGSV", 4) == 0) {
      snprintf(desp, sizeof(desp), "Stop go penalty served: %s",
               driver_name[m_eventDetails.StopGoPenaltyServed.vehicleIdx].name().c_str());
      snprintf(stmt, sizeof(stmt), fmt, PrimaryKeyCommonPart, i, "SGSV",
               driver_name[m_eventDetails.StopGoPenaltyServed.vehicleIdx].name().c_str(), desp);
    } else if (strncmp(m_eventStringCode, "FLBK", 4) == 0) {
      snprintf(desp, sizeof(desp), "Flashback activated: %u, %.f", m_eventDetails.Flashback.flashbackFrameIdentifier,
               m_eventDetails.Flashback.flashbackSessionTime);
      snprintf(stmt, sizeof(stmt), fmt, PrimaryKeyCommonPart, i, "FLBK", "-", desp);
    } else if (strncmp(m_eventStringCode, "BUTN", 4) == 0) {
      return std::string();
    }
    sql += stmt;
    return sql;
  }
};

#pragma pack(pop)
