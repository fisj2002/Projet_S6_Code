//Protocol
const byte prot_prefix = 'S';
const byte prot_suffix = 'E';

const byte prot_listComm = 'L' ;
const byte prot_sensorsComm = 'F' ;
const byte prot_onActComm = 'A';
const byte prot_offActComm = 'D';
const byte prot_newIdComm = 'X';
const byte prot_notifyComm = 'N';
const byte prot_sensorResponse = 'R';

const byte addrMaster = 0xFF;
const byte addrSlaveRand = 0x2A;
const byte addrSlaveConst = 0x37;

// The list of available slaves
const byte slaveList[2] = {addrSlaveRand, addrSlaveConst};

const unsigned long alertInterval = 5000;
unsigned long lastAlert = 0;

// The actuator of the random slave is to enable notifications every 5s
bool notificationEnable = false;

void setup() 
{
  pinMode(13,OUTPUT);
  
  Serial.begin(9600);
  
  Serial.println("Hello world");
  
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
}

void loop() 
{  
  // Check for new transmissions
  if(Serial.find(prot_prefix))
  {
    // Waiting for every bit to arrive
    delay(100);
    
    decodeIncoming();
  }
  
  // Sends a notification every 5 seconds
  if (millis() - lastAlert > alertInterval && notificationEnable)
  {
    slaveResponse(addrSlaveRand, prot_notifyComm);
    
    lastAlert = millis();
  }
}

void decodeIncoming()
{
  int messageSize = Serial.available();
    
  if (messageSize < 3)  // Shortest possible request is 3 words long
    return;
    
  byte destination = Serial.read();
  byte requestType = Serial.read();
  
  // Checking for master or slave request
  if (destination == addrMaster)
  {
    // Master requests
    if (requestType == prot_listComm)
      listResponse();
  }
  else
  {
    // Slave requests
    if (destination == addrSlaveRand || destination == addrSlaveConst)
    { // Slave Id checked
      
      if (requestType == prot_sensorsComm)
        slaveResponse(destination, prot_sensorResponse);
      else if (requestType == prot_onActComm)
      {
        if (destination == addrSlaveRand)
          notificationEnable = true;
        else
          digitalWrite(13, HIGH);
        slaveResponse(destination, prot_onActComm);
      }
      else if (requestType == prot_offActComm)
      {
        if (destination == addrSlaveRand)
          notificationEnable = false;
        else
          digitalWrite(13, LOW);
        slaveResponse(destination, prot_offActComm);
      }
    }
  }
}

void listResponse()
{
  Serial.write(prot_prefix);
  Serial.write(addrMaster);
  Serial.write(prot_listComm);
  Serial.write(sizeof(slaveList)/sizeof(byte));
  for (int i = 0; i < sizeof(slaveList)/sizeof(byte); ++i)
  {
    Serial.write(slaveList[i]);
  }
  Serial.write(prot_suffix);
}

void slaveResponse(byte slaveId, byte responseType)
{
  Serial.write(prot_prefix);
  Serial.write(slaveId);
  Serial.write(responseType);
  
  char temperature = 25;
  byte mouvement = 1;
  
  byte latitudeBytes[sizeof(float)];    // 45 deg
  byte longitudeBytes[sizeof(float)];   // 72 deg
  float* latitude = (float*)latitudeBytes;
  float* longitude = (float*)longitudeBytes;

  *latitude = 45.0;
  *longitude = 72.0;

  Serial.write(temperature);
  Serial.write(mouvement);
  Serial.write(longitudeBytes, sizeof(float));
  Serial.write(latitudeBytes, sizeof(float));
  
  Serial.write(prot_suffix);
}
