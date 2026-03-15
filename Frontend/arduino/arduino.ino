#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;
#include <DFRobot_SIM808.h>


#define CONNECT_BY_JUMPER   1
#if CONNECT_BY_JUMPER
  #define PIN_TX    10
  #define PIN_RX    11
  SoftwareSerial mySerial(PIN_TX, PIN_RX);
  DFRobot_SIM808 sim808(&mySerial);

#elif defined(ARDUINO_AVR_LEONARDO)
  DFRobot_SIM808 sim808(&Serial1);

#else
  DFRobot_SIM808 sim808(&Serial);
#endif
#define PHONE_NUMBER  "7972971539"

 sensors_event_t a, g, temp;

String latstr="18.4575";
String longstr="73.8508";
float previousVelocity = 0, currentVelocity = 0, t_acceleration;
uint32_t currentTime, previousTime;

void setup(void) {
  Serial.begin(9600);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  //setupt motion detection
  mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
  mpu.setMotionDetectionThreshold(1);
  mpu.setMotionDetectionDuration(20);
  mpu.setInterruptPinLatch(true);	// Keep it latched.  Will turn off when reinitialized.
  mpu.setInterruptPinPolarity(true);
  mpu.setMotionInterrupt(true);

  Serial.println("");
  delay(100);

  //call setup
  #if CONNECT_BY_JUMPER
    mySerial.begin(9600);
  #elif defined(ARDUINO_AVR_LEONARDO)
    Serial1.begin(9600);
  #endif
  Serial.begin(9600);

  //******** Initialize sim808 module *************
  while(!sim808.init()) {
      delay(1000);
      Serial.print("Sim808 init error\r\n");
  }  
  Serial.println("Sim808 init success");
  sim808.attachGPS();
}

void sosTrigger(){
  String MESSAGE = "Accident Detected! Location: https://www.google.com/maps/search/?api=1&query=" + String(latstr) + "," + String(longstr);
  MESSAGE.replace("query= ", "query=");
  sim808.sendSMS(PHONE_NUMBER, MESSAGE.c_str());
  Serial.print(MESSAGE);
}

void ShowSerialData() {
  while (mySerial.available() != 0)
    Serial.write(mySerial.read());
  delay(1000);
}

// void Put_thingspeak(){
//   Serial.println("IVE REACHED HERE");
//     // fileWrite();
//   if (mySerial.available())
//     Serial.write(mySerial.read());

//   mySerial.println("AT"); delay(1000);
//   mySerial.println("AT+CPIN?"); delay(1000);
//   mySerial.println("AT+CREG?"); delay(1000);
//   mySerial.println("AT+CGATT?"); delay(1000);
//   mySerial.println("AT+CIPSHUT"); delay(1000);
//   mySerial.println("AT+CIPSTATUS"); delay(2000);
//   mySerial.println("AT+CIPMUX=0"); delay(2000);
//   ShowSerialData();
//   mySerial.println("AT+CSTT=\"dialogbb\"");  //start task and setting the APN,
//   delay(1000);
//   ShowSerialData();

//   mySerial.println("AT+CIICR");  //bring up wireless connection
//   delay(3000);
//   ShowSerialData();

//   mySerial.println("AT+CIFSR");  //get local IP adress
//   delay(2000);
//   ShowSerialData();

//   mySerial.println("AT+CIPSPRT=0"); delay(3000);
//   ShowSerialData();

//   mySerial.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");  //start up the connection
//   delay(2000);
//   ShowSerialData();

//   mySerial.println("AT+CIPSEND");  //begin send data to remote server
//   delay(4000);
//   ShowSerialData();

//   String str = "GET https://api.thingspeak.com/update?api_key=LABMYNDB3JJJ31LH&field1=" + 
//         String(latstr) + "&field2=" + String(longstr) + 
//         "&field3="+String(temp.temperature) +
//         "&field4="+String(currentVelocity);
//   str.replace("field1= ", "field1=");  // remove the space between the latitude/longitude and "&field3"

//   Serial.println(str);
//   mySerial.println(str);  //begin send data to remote server
//   delay(3000);
//   ShowSerialData();

//   mySerial.println((char)26);  //sending
//   delay(2000);                 //waitting for reply, important! the time is base on the condition of internet
//   mySerial.println();
//   Serial.println("REACHED");
  
// }

void Put_thingspeak() {
  // fileWrite();
   if (mySerial.available())
    Serial.write(mySerial.read());

  mySerial.println("AT");
  delay(1000);

  mySerial.println("AT+CPIN?");
  delay(1000);

  mySerial.println("AT+CREG?");
  delay(1000);

  mySerial.println("AT+CGATT?");
  delay(1000);

  mySerial.println("AT+CIPSHUT");
  delay(1000);

  mySerial.println("AT+CIPSTATUS");
  delay(2000);

  mySerial.println("AT+CIPMUX=0");
  delay(2000);

  ShowSerialData();

  mySerial.println("AT+CSTT=\"dialogbb\"");  //start task and setting the APN,
  delay(1000);

  ShowSerialData();

  mySerial.println("AT+CIICR");  //bring up wireless connection
  delay(3000);

  ShowSerialData();

  mySerial.println("AT+CIFSR");  //get local IP adress
  delay(2000);

  ShowSerialData();

  mySerial.println("AT+CIPSPRT=0");
  delay(3000);

  ShowSerialData();

  mySerial.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");  //start up the connection
  delay(2000);

  ShowSerialData();

  mySerial.println("AT+CIPSEND");  //begin send data to remote server
  delay(4000);
  ShowSerialData();

  String str = "GET https://api.thingspeak.com/update?api_key=RSFSYWVWWGANEP19&field1="+String(latstr)+"&field2="+String(longstr)+"&field3="+String(temp.temperature)+"&field4="+String(currentVelocity);
  str.replace("field1= ", "field1=");  // remove the space between the latitude/longitude and "&field3"

  Serial.println(str);
  mySerial.println(str);  //begin send data to remote server

  delay(4000);
  ShowSerialData();

  mySerial.println((char)26);  //sending
  delay(2000);                 //waitting for reply, important! the time is base on the condition of internet
  mySerial.println();

  ShowSerialData();

  mySerial.println("AT+CIPSHUT");  //close the connection
  delay(100);
  ShowSerialData();
  delay(2000);

 
}


void loop() {
    /* Get new sensor events with the readings */
    // if(sim8)
    mpu.getEvent(&a, &g, &temp);

    /* Print out the values */
    Serial.println(""); 
    Serial.print("AccelX:"); Serial.print(a.acceleration.x); Serial.print(",");
    Serial.print("AccelY:"); Serial.print(a.acceleration.y); Serial.print(",");
    Serial.print("AccelZ:"); Serial.print(a.acceleration.z); Serial.print(", ");
    Serial.print("GyroX:");  Serial.print(g.gyro.x);  Serial.print(",");
    Serial.print("GyroY:");  Serial.print(g.gyro.y);  Serial.print(",");
    Serial.print("GyroZ:");  Serial.print(g.gyro.z);  Serial.println("");
    Serial.print("temp:");   Serial.println(temp.temperature);

    currentVelocity = previousVelocity + sqrt(
      a.acceleration.x * a.acceleration.x + a.acceleration.y * a.acceleration.y + a.acceleration.z * a.acceleration.z
    ) * 0.2;

    Serial.print("VELOCITY: ");  Serial.println(currentVelocity);

    // Check for motion detection thresholds
    int a_thresh = 80; // Acceleration threshold
    int g_thresh = 4; // Gyroscope threshold

    if (abs(a.acceleration.x) > a_thresh || abs(a.acceleration.y) > a_thresh || abs(a.acceleration.z) > a_thresh ||
        abs(g.gyro.x) > g_thresh || abs(g.gyro.y) > g_thresh || abs(g.gyro.z) > g_thresh) {
      Serial.println("Accident Detected");
      sosTrigger();
      Put_thingspeak();
      
      // Add additional actions here if needed
    }
    
  
  delay(200);
}