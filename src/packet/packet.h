#pragma once

#include "car_damage.h"
#include "car_motion.h"
#include "car_setup.h"
#include "car_status.h"
#include "car_telemetry.h"
#include "event.h"
#include "final_classification.h"
#include "lap_data.h"
#include "lobby_info.h"
#include "packet_header.h"
#include "participants.h"
#include "session.h"
#include "session_history.h"

#pragma pack(push, 1)
union Packet {
  PacketHeader header;
  PacketMotionData CarMotion;
  PacketSessionData SessionData;
  PacketLapData LapData;
  PacketEventData EventData;
  PacketParticipantsData Participants;
  PacketCarSetupData CarSetup;
  PacketCarTelemetryData CarTelemetry;
  PacketCarStatusData CarStatus;
  PacketFinalClassificationData FinalClassification;
  PacketLobbyInfoData LobbyInfo;
  PacketCarDamageData CarDamage;
  PacketSessionHistoryData SessionHistory;

  char raw_data[2048];

  Packet() { memset(this, 0, sizeof(raw_data)); }
};
#pragma pack(pop)