use f1_2022;

CREATE TABLE IF NOT EXISTS CarStatus (
    beginUnixTime       INT UNSIGNED,
    curUnixTime         INT UNSIGNED,
    curTime             DATETIME,

    carIndex            TINYINT UNSIGNED,
    driverName          VARCHAR(48),

    tractionControl    	    TINYINT UNSIGNED,     
    antiLockBrakes          TINYINT UNSIGNED,   
    fuelMix                 TINYINT UNSIGNED,
    fuelMixInStr            CHAR(10),
    frontBrakeBias          TINYINT UNSIGNED,     
    pitLimiterStatus        TINYINT UNSIGNED,   
    fuelInTank              FLOAT,    
    fuelCapacity            FLOAT,  
    fuelRemainingLaps       FLOAT,   
    maxRPM                  SMALLINT  UNSIGNED,       
    idleRPM                 SMALLINT  UNSIGNED,        
    maxGears                TINYINT UNSIGNED,   
    drsAllowed              TINYINT UNSIGNED,
    drsActivationDistance   SMALLINT  UNSIGNED,                     
    actualTyreCompound      TINYINT UNSIGNED,
    visualTyreCompound      TINYINT UNSIGNED,                
    tyresAgeLaps            TINYINT UNSIGNED,  
    vehicleFiaFlags         TINYINT,
    vehicleFiaFlagsInStr    CHAR(10),             
    ersStoreEnergy          FLOAT,   
    ersDeployMode           TINYINT UNSIGNED,
    ersDeployModeInStr      CHAR(10),                    
    ersHarvestedThisLapMGUK FLOAT,
    ersHarvestedThisLapMGUH FLOAT,
    ersDeployedThisLap      FLOAT,
    networkPaused           TINYINT UNSIGNED,   		

    PRIMARY KEY(curUnixTime, carIndex),
    KEY(beginUnixTime, carIndex),
    FOREIGN KEY (beginUnixTime) REFERENCES SessionList(beginUnixTime)
);

