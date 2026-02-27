#pragma once

#include <cstdint>
#include <map>
#include <string>

namespace f1_2022 {

// TeamIDs
const std::map<uint8_t, std::string> TeamIDs = {
    {0, "Mercedes"},
    {106, "Prema '21"},
    {1, "Ferrari"},
    {107, "Uni-Virtuosi '21"},
    {2, "Red Bull Racing"},
    {108, "Carlin '21"},
    {3, "Williams"},
    {109, "Hitech '21"},
    {4, "Aston Martin"},
    {110, "Art GP '21"},
    {5, "Alpine"},
    {111, "MP Motorsport '21"},
    {6, "Alpha Tauri"},
    {112, "Charouz '21"},
    {7, "Haas"},
    {113, "Dams '21"},
    {8, "McLaren"},
    {114, "Campos '21"},
    {9, "Alfa Romeo"},
    {115, "BWT '21"},
    {85, "Mercedes 2020"},
    {116, "Trident '21"},
    {86, "Ferrari 2020"},
    {117, "Mercedes AMG GT Black Series"},
    {87, "Red Bull 2020"},
    {118, "Prema '22"},
    {88, "Williams 2020"},
    {119, "Virtuosi '22"},
    {89, "Racing Point 2020"},
    {120, "Carlin '22"},
    {90, "Renault 2020"},
    {121, "Hitech '22"},
    {91, "Alpha Tauri 2020"},
    {122, "Art GP '22"},
    {92, "Haas 2020"},
    {123, "MP Motorsport '22"},
    {93, "McLaren 2020"},
    {124, "Charouz '22"},
    {94, "Alfa Romeo 2020"},
    {125, "Dams '22"},
    {95, "Aston Martin DB11 V12"},
    {126, "Campos '22"},
    {96, "Aston Martin Vantage F1 Edition"},
    {127, "Van Amersfoort Racing '22"},
    {97, "Aston Martin Vantage Safety Car"},
    {128, "Trident '22"},
    {98, "Ferrari F8 Tributo"},
    {99, "Ferrari Roma"},
    {100, "McLaren 720S"},
    {101, "McLaren Artura"},
    {102, "Mercedes AMG GT Black Series Safety Car"},
    {103, "Mercedes AMG GTR Pro"},
    {104, "F1 Custom Team"},
};

// DriverIDs
const std::map<uint8_t, std::string> DriverIDs = {
    {0, "Carlos Sainz"},
    {56, "Louis Delétraz"},
    {115, "Theo Pourchaire"},
    {1, "Daniil Kvyat"},
    {57, "Antonio Fuoco"},
    {116, "Richard Verschoor"},
    {2, "Daniel Ricciardo"},
    {58, "Charles Leclerc"},
    {117, "Lirim Zendeli"},
    {3, "Fernando Alonso"},
    {59, "Pierre Gasly"},
    {118, "David Beckmann"},
    {4, "Felipe Massa"},
    {62, "Alexander Albon"},
    {121, "Alessio Deledda"},
    {6, "Kimi Räikkönen"},
    {63, "Nicholas Latifi"},
    {122, "Bent Viscaal"},
    {7, "Lewis Hamilton"},
    {64, "Dorian Boccolacci"},
    {123, "Enzo Fittipaldi"},
    {9, "Max Verstappen"},
    {65, "Niko Kari"},
    {125, "Mark Webber"},
    {10, "Nico Hulkenburg"},
    {66, "Roberto Merhi"},
    {126, "Jacques Villeneuve"},
    {11, "Kevin Magnussen"},
    {67, "Arjun Maini"},
    {127, "Jake Hughes"},
    {12, "Romain Grosjean"},
    {68, "Alessio Lorandi"},
    {128, "Frederik Vesti"},
    {13, "Sebastian Vettel"},
    {69, "Ruben Meijer"},
    {129, "Olli Caldwell"},
    {14, "Sergio Perez"},
    {70, "Rashid Nair"},
    {130, "Logan Sargeant"},
    {15, "Valtteri Bottas"},
    {71, "Jack Tremblay"},
    {131, "Cem Bolukbasi"},
    {17, "Esteban Ocon"},
    {72, "Devon Butler"},
    {132, "Ayuma Iwasa"},
    {19, "Lance Stroll"},
    {73, "Lukas Weber"},
    {133, "Clement Novolak"},
    {20, "Arron Barnes"},
    {74, "Antonio Giovinazzi"},
    {134, "Dennis Hauger"},
    {21, "Martin Giles"},
    {75, "Robert Kubica"},
    {135, "Calan Williams"},
    {22, "Alex Murray"},
    {76, "Alain Prost"},
    {136, "Jack Doohan"},
    {23, "Lucas Roth"},
    {77, "Ayrton Senna"},
    {137, "Amaury Cordeel"},
    {24, "Igor Correia"},
    {78, "Nobuharu Matsushita"},
    {138, "Mika Hakkinen"},
    {25, "Sophie Levasseur"},
    {79, "Nikita Mazepin"},
    {26, "Jonas Schiffer"},
    {80, "Guanya Zhou"},
    {27, "Alain Forest"},
    {81, "Mick Schumacher"},
    {28, "Jay Letourneau"},
    {82, "Callum Ilott"},
    {29, "Esto Saari"},
    {83, "Juan Manuel Correa"},
    {30, "Yasar Atiyeh"},
    {84, "Jordan King"},
    {31, "Callisto Calabresi"},
    {85, "Mahaveer Raghunathan"},
    {32, "Naota Izum"},
    {86, "Tatiana Calderon"},
    {33, "Howard Clarke"},
    {87, "Anthoine Hubert"},
    {34, "Wilheim Kaufmann"},
    {88, "Guiliano Alesi"},
    {35, "Marie Laursen"},
    {89, "Ralph Boschung"},
    {36, "Flavio Nieves"},
    {90, "Michael Schumacher"},
    {37, "Peter Belousov"},
    {91, "Dan Ticktum"},
    {38, "Klimek Michalski"},
    {92, "Marcus Armstrong"},
    {39, "Santiago Moreno"},
    {93, "Christian Lundgaard"},
    {40, "Benjamin Coppens"},
    {94, "Yuki Tsunoda"},
    {41, "Noah Visser"},
    {95, "Jehan Daruvala"},
    {42, "Gert Waldmuller"},
    {96, "Gulherme Samaia"},
    {43, "Julian Quesada"},
    {97, "Pedro Piquet"},
    {44, "Daniel Jones"},
    {98, "Felipe Drugovich"},
    {45, "Artem Markelov"},
    {99, "Robert Schwartzman"},
    {46, "Tadasuke Makino"},
    {100, "Roy Nissany"},
    {47, "Sean Gelael"},
    {101, "Marino Sato"},
    {48, "Nyck De Vries"},
    {102, "Aidan Jackson"},
    {49, "Jack Aitken"},
    {103, "Casper Akkerman"},
    {50, "George Russell"},
    {109, "Jenson Button"},
    {51, "Maximilian Günther"},
    {110, "David Coulthard"},
    {52, "Nirei Fukuzumi"},
    {111, "Nico Rosberg"},
    {53, "Luca Ghiotto"},
    {112, "Oscar Piastri"},
    {54, "Lando Norris"},
    {113, "Liam Lawson"},
    {55, "Sérgio Sette Câmara"},
    {114, "Juri Vips"},
};

// TrackIDs
const std::map<uint8_t, std::string> TrackIDs = {
    {0, "Melbourne"},
    {1, "Paul Ricard"},
    {2, "Shanghai"},
    {3, "Sakhir (Bahrain)"},
    {4, "Catalunya"},
    {5, "Monaco"},
    {6, "Montreal"},
    {7, "Silverstone"},
    {8, "Hockenheim"},
    {9, "Hungaroring"},
    {10, "Spa"},
    {11, "Monza"},
    {12, "Singapore"},
    {13, "Suzuka"},
    {14, "Abu Dhabi"},
    {15, "Texas"},
    {16, "Brazil"},
    {17, "Austria"},
    {18, "Sochi"},
    {19, "Mexico"},
    {20, "Baku (Azerbaijan)"},
    {21, "Sakhir Short"},
    {22, "Silverstone Short"},
    {23, "Texas Short"},
    {24, "Suzuka Short"},
    {25, "Hanoi"},
    {26, "Zandvoort"},
    {27, "Imola"},
    {28, "Portimão"},
    {29, "Jeddah"},
    {30, "Miami"},
};

// NationalityIDs
const std::map<uint8_t, std::string> NationalityIDs = {
    {1, "American"},
    {31, "Greek"},
    {61, "Paraguayan"},
    {2, "Argentinean"},
    {32, "Guatemalan"},
    {62, "Peruvian"},
    {3, "Australian"},
    {33, "Honduran"},
    {63, "Polish"},
    {4, "Austrian"},
    {34, "Hong Konger"},
    {64, "Portuguese"},
    {5, "Azerbaijani"},
    {35, "Hungarian"},
    {65, "Qatari"},
    {6, "Bahraini"},
    {36, "Icelander"},
    {66, "Romanian"},
    {7, "Belgian"},
    {37, "Indian"},
    {67, "Russian"},
    {8, "Bolivian"},
    {38, "Indonesian"},
    {68, "Salvadoran"},
    {9, "Brazilian"},
    {39, "Irish"},
    {69, "Saudi"},
    {10, "British"},
    {40, "Israeli"},
    {70, "Scottish"},
    {11, "Bulgarian"},
    {41, "Italian"},
    {71, "Serbian"},
    {12, "Cameroonian"},
    {42, "Jamaican"},
    {72, "Singaporean"},
    {13, "Canadian"},
    {43, "Japanese"},
    {73, "Slovakian"},
    {14, "Chilean"},
    {44, "Jordanian"},
    {74, "Slovenian"},
    {15, "Chinese"},
    {45, "Kuwaiti"},
    {75, "South Korean"},
    {16, "Colombian"},
    {46, "Latvian"},
    {76, "South African"},
    {17, "Costa Rican"},
    {47, "Lebanese"},
    {77, "Spanish"},
    {18, "Croatian"},
    {48, "Lithuanian"},
    {78, "Swedish"},
    {19, "Cypriot"},
    {49, "Luxembourger"},
    {79, "Swiss"},
    {20, "Czech"},
    {50, "Malaysian"},
    {80, "Thai"},
    {21, "Danish"},
    {51, "Maltese"},
    {81, "Turkish"},
    {22, "Dutch"},
    {52, "Mexican"},
    {82, "Uruguayan"},
    {23, "Ecuadorian"},
    {53, "Monegasque"},
    {83, "Ukrainian"},
    {24, "English"},
    {54, "New Zealander"},
    {84, "Venezuelan"},
    {25, "Emirian"},
    {55, "Nicaraguan"},
    {85, "Barbadian"},
    {26, "Estonian"},
    {56, "Northern Irish"},
    {86, "Welsh"},
    {27, "Finnish"},
    {57, "Norwegian"},
    {87, "Vietnamese"},
    {28, "French"},
    {58, "Omani"},
    {29, "German"},
    {59, "Pakistani"},
    {30, "Ghanaian"},
    {60, "Panamanian"},
};

// GameModeIDs
const std::map<uint8_t, std::string> GameModeIDs = {
    {0, "Event Mode"},
    {3, "Grand Prix"},
    {5, "Time Trial"},
    {6, "Splitscreen"},
    {7, "Online Custom"},
    {8, "Online League"},
    {11, "Career Invitational"},
    {12, "Championship Invitational"},
    {13, "Championship"},
    {14, "Online Championship"},
    {15, "Online Weekly Event"},
    {19, "Career '22"},
    {20, "Career '22 Online"},
    {127, "Benchmark"},
};

// RulesetIDs
const std::map<uint8_t, std::string> RulesetIDs = {
    {0, "Practice & Qualifying"},
    {1, "Race"},
    {2, "Time Trial"},
    {4, "Time Attack"},
    {6, "Checkpoint Challenge"},
    {8, "Autocross"},
    {9, "Drift"},
    {10, "Average Speed Zone"},
    {11, "Rival Duel"},
};

// SurfaceTypes
const std::map<uint8_t, std::string> SurfaceTypes = {
    {0, "Tarmac"},
    {1, "Rumble strip"},
    {2, "Concrete"},
    {3, "Rock"},
    {4, "Gravel"},
    {5, "Mud"},
    {6, "Sand"},
    {7, "Grass"},
    {8, "Water"},
    {9, "Cobblestone"},
    {10, "Metal"},
    {11, "Ridged"},
};

// ButtonFlags
const std::map<uint32_t, std::string> ButtonFlags = {
    {0x00000001, "Cross or A"},
    {0x00000002, "Triangle or Y"},
    {0x00000004, "Circle or B"},
    {0x00000008, "Square or X"},
    {0x00000010, "D-pad Left"},
    {0x00000020, "D-pad Right"},
    {0x00000040, "D-pad Up"},
    {0x00000080, "D-pad Down"},
    {0x00000100, "Options or Menu"},
    {0x00000200, "L1 or LB"},
    {0x00000400, "R1 or RB"},
    {0x00000800, "L2 or LT"},
    {0x00001000, "R2 or RT"},
    {0x00002000, "Left Stick Click"},
    {0x00004000, "Right Stick Click"},
    {0x00008000, "Right Stick Left"},
    {0x00010000, "Right Stick Right"},
    {0x00020000, "Right Stick Up"},
    {0x00040000, "Right Stick Down"},
    {0x00080000, "Special"},
    {0x00100000, "UDP Action 1"},
    {0x00200000, "UDP Action 2"},
    {0x00400000, "UDP Action 3"},
    {0x00800000, "UDP Action 4"},
    {0x01000000, "UDP Action 5"},
    {0x02000000, "UDP Action 6"},
    {0x04000000, "UDP Action 7"},
    {0x08000000, "UDP Action 8"},
    {0x10000000, "UDP Action 9"},
    {0x20000000, "UDP Action 10"},
    {0x40000000, "UDP Action 11"},
    {0x80000000, "UDP Action 12"},
};

// PenaltyTypes
const std::map<uint8_t, std::string> PenaltyTypes = {
    {0, "Drive through"},
    {1, "Stop Go"},
    {2, "Grid penalty"},
    {3, "Penalty reminder"},
    {4, "Time penalty"},
    {5, "Warning"},
    {6, "Disqualified"},
    {7, "Removed from formation lap"},
    {8, "Parked too long timer"},
    {9, "Tyre regulations"},
    {10, "This lap invalidated"},
    {11, "This and next lap invalidated"},
    {12, "This lap invalidated without reason"},
    {13, "This and next lap invalidated without reason"},
    {14, "This and previous lap invalidated"},
    {15, "This and previous lap invalidated without reason"},
    {16, "Retired"},
    {17, "Black flag timer"},
};

// InfringementTypes
const std::map<uint8_t, std::string> InfringementTypes = {
    {0, "Blocking by slow driving"},
    {1, "Blocking by wrong way driving"},
    {2, "Reversing off the start line"},
    {3, "Big Collision"},
    {4, "Small Collision"},
    {5, "Collision failed to hand back position single"},
    {6, "Collision failed to hand back position multiple"},
    {7, "Corner cutting gained time"},
    {8, "Corner cutting overtake single"},
    {9, "Corner cutting overtake multiple"},
    {10, "Crossed pit exit lane"},
    {11, "Ignoring blue flags"},
    {12, "Ignoring yellow flags"},
    {13, "Ignoring drive through"},
    {14, "Too many drive throughs"},
    {15, "Drive through reminder serve within n laps"},
    {16, "Drive through reminder serve this lap"},
    {17, "Pit lane speeding"},
    {18, "Parked for too long"},
    {19, "Ignoring tyre regulations"},
    {20, "Too many penalties"},
    {21, "Multiple warnings"},
    {22, "Approaching disqualification"},
    {23, "Tyre regulations select single"},
    {24, "Tyre regulations select multiple"},
    {25, "Lap invalidated corner cutting"},
    {26, "Lap invalidated running wide"},
    {27, "Corner cutting ran wide gained time minor"},
    {28, "Corner cutting ran wide gained time significant"},
    {29, "Corner cutting ran wide gained time extreme"},
    {30, "Lap invalidated wall riding"},
    {31, "Lap invalidated flashback used"},
    {32, "Lap invalidated reset to track"},
    {33, "Blocking the pitlane"},
    {34, "Jump start"},
    {35, "Safety car to car collision"},
    {36, "Safety car illegal overtake"},
    {37, "Safety car exceeding allowed pace"},
    {38, "Virtual safety car exceeding allowed pace"},
    {39, "Formation lap below allowed speed"},
    {40, "Formation lap parking"},
    {41, "Retired mechanical failure"},
    {42, "Retired terminally damaged"},
    {43, "Safety car falling too far back"},
    {44, "Black flag timer"},
    {45, "Unserved stop go penalty"},
    {46, "Unserved drive through penalty"},
    {47, "Engine component change"},
    {48, "Gearbox change"},
    {49, "Parc Fermé change"},
    {50, "League grid penalty"},
    {51, "Retry penalty"},
    {52, "Illegal time gain"},
    {53, "Mandatory pitstop"},
    {54, "Attribute assigned"},
};

// ActualTyreCompound
const std::map<uint8_t, std::string> ActualTyreCompound = {
    {7, "inter"},
    {8, "wet F1 Classic -"},
    {9, "dry"},
    {10, "wet F2"},
    {11, "super soft"},
    {12, "soft"},
    {13, "medium"},
    {14, "hard"},
    {15, "wet"},
    {16, "C5"},
    {17, "C4"},
    {18, "C3"},
    {19, "C2"},
    {20, "C1"},
};

// AirTemperatureChange
const std::map<uint8_t, std::string> AirTemperatureChange = {
    {0, "up"},
    {1, "down"},
    {2, "no change"},
};

// BrakingAssist
const std::map<uint8_t, std::string> BrakingAssist = {
    {0, "off"},
    {1, "low"},
    {2, "medium"},
    {3, "high"},
};

// CurrentLapInvalid
const std::map<uint8_t, std::string> CurrentLapInvalid = {
    {0, "valid"},
    {1, "invalid"},
};

// DRSAssist
const std::map<uint8_t, std::string> DRSAssist = {
    {0, "off"},
    {1, "on"},
};

// DriverStatus
const std::map<uint8_t, std::string> DriverStatus = {
    {0, "in garage"},
    {1, "flying lap"},
    {2, "in lap"},
    {3, "out lap"},
    {4, "on track"},
};

// Drs
const std::map<uint8_t, std::string> Drs = {
    {0, "off"},
    {1, "on"},
};

// DrsActivationDistance
const std::map<uint8_t, std::string> DrsActivationDistance = {
    {0, "DRS not available, non-zero - DRS will be available in [X] metres"},
};

// DrsAllowed
const std::map<uint8_t, std::string> DrsAllowed = {
    {0, "not allowed"},
    {1, "allowed"},
};

// DrsFault
const std::map<uint8_t, std::string> DrsFault = {
    {0, "OK"},
    {1, "fault"},
};

// DynamicRacingLine
const std::map<uint8_t, std::string> DynamicRacingLine = {
    {0, "off"},
    {1, "corners only"},
    {2, "full"},
};

// DynamicRacingLineType
const std::map<uint8_t, std::string> DynamicRacingLineType = {
    {0, "2D"},
    {1, "3D"},
};

// ERSAssist
const std::map<uint8_t, std::string> ERSAssist = {
    {0, "off"},
    {1, "on"},
};

// EngineBlown
const std::map<uint8_t, std::string> EngineBlown = {
    {0, "OK"},
    {1, "fault"},
};

// EngineSeized
const std::map<uint8_t, std::string> EngineSeized = {
    {0, "OK"},
    {1, "fault"},
};

// ErsDeployMode
const std::map<uint8_t, std::string> ErsDeployMode = {
    {0, "none"},
    {1, "medium"},
    {2, "hotlap"},
    {3, "overtake"},
};

// ErsFault
const std::map<uint8_t, std::string> ErsFault = {
    {0, "OK"},
    {1, "fault"},
};

// ForecastAccuracy
const std::map<uint8_t, std::string> ForecastAccuracy = {
    {0, "Perfect"},
    {1, "Approximate"},
};

// Formula
const std::map<uint8_t, std::string> Formula = {
    {0, "F1 Modern"},
    {1, "F1 Classic"},
    {2, "F2"},
    {3, "F1 Generic"},
    {4, "Beta"},
    {5, "Supercars"},
    {6, "Esports"},
    {7, "F2 2021"},
};

// FuelMix
const std::map<uint8_t, std::string> FuelMix = {
    {0, "lean"},
    {1, "standard"},
    {2, "rich"},
    {3, "max"},
};

// GearboxAssist
const std::map<uint8_t, std::string> GearboxAssist = {
    {1, "manual"},
    {2, "manual & suggested gear"},
    {3, "auto"},
};

// MfdPanelIndex
const std::map<uint8_t, std::string> MfdPanelIndex = {
    {0, "Car setup"},
    {1, "Pits"},
    {2, "Damage"},
    {3, "Engine"},
    {4, "Temperatures May vary depending on game mode"},
    {255, "MFD closed Single player, race"},
};

// MyTeam
const std::map<uint8_t, std::string> MyTeam = {
    {0, "otherwise"},
    {1, "My Team"},
};

// NetworkGame
const std::map<uint8_t, std::string> NetworkGame = {
    {0, "offline"},
    {1, "online"},
};

// PitAssist
const std::map<uint8_t, std::string> PitAssist = {
    {0, "off"},
    {1, "on"},
};

// PitLaneTimerActive
const std::map<uint8_t, std::string> PitLaneTimerActive = {
    {0, "inactive"},
    {1, "active"},
};

// PitLimiterStatus
const std::map<uint8_t, std::string> PitLimiterStatus = {
    {0, "off"},
    {1, "on"},
};

// PitReleaseAssist
const std::map<uint8_t, std::string> PitReleaseAssist = {
    {0, "off"},
    {1, "on"},
};

// PitStatus
const std::map<uint8_t, std::string> PitStatus = {
    {0, "none"},
    {1, "pitting"},
    {2, "in pit area"},
};

// ReadyStatus
const std::map<uint8_t, std::string> ReadyStatus = {
    {0, "not ready"},
    {1, "ready"},
    {2, "spectating"},
};

// ResultStatus
const std::map<uint8_t, std::string> ResultStatus = {
    {0, "invalid"},
    {1, "inactive"},
    {2, "active"},
    {3, "finished"},
    {4, "didnotfinish"},
    {5, "disqualified"},
    {6, "not classified"},
    {7, "retired"},
};

// RevLightsBitValue
const std::map<uint8_t, std::string> RevLightsBitValue = {
    {0, "leftmost LED, bit"},
    {14, "rightmost LED)"},
};

// SafetyCarStatus
const std::map<uint8_t, std::string> SafetyCarStatus = {
    {0, "no safety car"},
    {1, "full"},
    {2, "virtual"},
    {3, "formation lap"},
};

// Sector
const std::map<uint8_t, std::string> Sector = {
    {0, "sector1"},
    {1, "sector2"},
    {2, "sector3"},
};

// SessionLength
const std::map<uint8_t, std::string> SessionLength = {
    {0, "None"},
    {2, "Very Short"},
    {3, "Short"},
    {4, "Medium"},
    {5, "Medium Long"},
    {6, "Long"},
    {7, "Full"},
};

// SessionType
const std::map<uint8_t, std::string> SessionType = {
    {0, "unknown"},
    {1, "P1"},
    {2, "P2"},
    {3, "P3"},
    {4, "Short P"},
    {5, "Q1"},
    {6, "Q2"},
    {7, "Q3"},
    {8, "Short Q"},
    {9, "OSQ"},
    {10, "R"},
    {11, "R2"},
    {12, "R3"},
    {13, "Time Trial"},
};

// SliProNativeSupport
const std::map<uint8_t, std::string> SliProNativeSupport = {
    {0, "inactive"},
    {1, "active"},
};

// SteeringAssist
const std::map<uint8_t, std::string> SteeringAssist = {
    {0, "off"},
    {1, "on"},
};

// TrackTemperatureChange
const std::map<uint8_t, std::string> TrackTemperatureChange = {
    {0, "up"},
    {1, "down"},
    {2, "no change"},
};

// TractionControl
const std::map<uint8_t, std::string> TractionControl = {
    {0, "off"},
    {1, "medium"},
    {2, "full"},
};

// VehicleFiaFlags
const std::map<int8_t, std::string> VehicleFiaFlags = {
    {-1, "invalid/unknown"},
    {0, "none"},
    {1, "green"},
    {2, "blue"},
    {3, "yellow"},
    {4, "red"},
};

// VisualTyreCompound
const std::map<uint8_t, std::string> VisualTyreCompound = {
    {7, "inter"},
    {8, "wet F1 Classic  same as above F2 19"},
    {15, "wet, 19  super soft"},
    {16, "soft"},
    {17, "medium"},
    {18, "hard"},
    {20, "soft"},
    {21, "medium"},
    {22, "hard"},
};

// Weather
const std::map<uint8_t, std::string> Weather = {
    {0, "clear"},
    {1, "light cloud"},
    {2, "overcast"},
    {3, "light rain"},
    {4, "heavy rain"},
    {5, "storm"},
};

// YourTelemetry
const std::map<uint8_t, std::string> YourTelemetry = {
    {0, "restricted"},
    {1, "public"},
};

// ZoneFlag
const std::map<int8_t, std::string> ZoneFlag = {
    {-1, "invalid/unknown"},
    {0, "none"},
    {1, "green"},
    {2, "blue"},
    {3, "yellow"},
    {4, "red"},
};

} // namespace f1_2022