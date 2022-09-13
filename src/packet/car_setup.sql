use f1_2022;

CREATE TABLE IF NOT EXISTS CarSetup (
    beginUnixTime       INT UNSIGNED,
    curUnixTime         INT UNSIGNED,
    curTime             DATETIME,

    carIndex            TINYINT UNSIGNED,
    driverName          VARCHAR(48),

    frontWing           TINYINT UNSIGNED,             
    rearWing            TINYINT UNSIGNED,             
    onThrottle          TINYINT UNSIGNED,            
    offThrottle         TINYINT UNSIGNED,           
    frontCamber         FLOAT,           
    rearCamber          FLOAT,            
    frontToe            FLOAT,              
    rearToe             FLOAT,               
    frontSuspension     TINYINT UNSIGNED,  
    rearSuspension      TINYINT UNSIGNED,    
    frontAntiRollBar    TINYINT UNSIGNED,    
    rearAntiRollBar     TINYINT UNSIGNED,   
    frontSuspensionHeight   TINYINT UNSIGNED,    
    rearSuspensionHeight    TINYINT UNSIGNED,   
    brakePressure           TINYINT UNSIGNED,   
    brakeBias               TINYINT UNSIGNED,
    rearLeftTyrePressure    FLOAT,
    rearRightTyrePressure   FLOAT,
    frontLeftTyrePressure   FLOAT,
    frontRightTyrePressure  FLOAT,
    ballast             TINYINT UNSIGNED,
    fuelLoad            FLOAT, 
    
    PRIMARY KEY(curUnixTime, carIndex),
    KEY(beginUnixTime, carIndex),
    FOREIGN KEY (beginUnixTime) REFERENCES SessionList(beginUnixTime)
);

          