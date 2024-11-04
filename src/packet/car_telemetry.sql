CREATE DATABASE IF NOT EXISTS f1_2022_tele;
USE f1_2022_tele;

CREATE TABLE IF NOT EXISTS CarTelemetry (
    ipDecimal           INT UNSIGNED,
    beginUnixTime       INT UNSIGNED,
    curUnixTime         INT UNSIGNED,
    curTime             DATETIME,

    carIndex            TINYINT UNSIGNED,
    driverName          VARCHAR(48),

    speed               SMALLINT UNSIGNED,                   
    throttle            FLOAT,                  
    steer               FLOAT,                 
    brake               FLOAT,                   
    clutch              TINYINT UNSIGNED,                    
    gear                TINYINT,                       
    engineRPM           SMALLINT UNSIGNED,                
    drs                 TINYINT UNSIGNED,                      
    revLightsPercent    TINYINT UNSIGNED,           
    revLightsBitValue   SMALLINT UNSIGNED,     
    brakesTemperatureRL  SMALLINT UNSIGNED,
    brakesTemperatureRR  SMALLINT UNSIGNED,   
    brakesTemperatureFL  SMALLINT UNSIGNED,   
    brakesTemperatureFR  SMALLINT UNSIGNED,           
    tyresSurfaceTemperatureRL TINYINT UNSIGNED,
    tyresSurfaceTemperatureRR TINYINT UNSIGNED, 
    tyresSurfaceTemperatureFL TINYINT UNSIGNED, 
    tyresSurfaceTemperatureFR TINYINT UNSIGNED,  
    tyresInnerTemperatureRL  TINYINT UNSIGNED,
    tyresInnerTemperatureRR  TINYINT UNSIGNED, 
    tyresInnerTemperatureFL  TINYINT UNSIGNED, 
    tyresInnerTemperatureFR  TINYINT UNSIGNED, 
    engineTemperature        SMALLINT UNSIGNED, 
    tyresPressureRL           FLOAT,
    tyresPressureRR           FLOAT,
    tyresPressureFL           FLOAT,
    tyresPressureFR           FLOAT,
    surfaceTypeRL            TINYINT UNSIGNED,
    surfaceTypeRR            TINYINT UNSIGNED,
    surfaceTypeFL            TINYINT UNSIGNED,
    surfaceTypeFR            TINYINT UNSIGNED,
    
    PRIMARY KEY(ipDecimal, beginUnixTime, curUnixTime, carIndex),
    FOREIGN KEY(ipDecimal, beginUnixTime) REFERENCES SessionList(ipDecimal, beginUnixTime) ON DELETE CASCADE
);