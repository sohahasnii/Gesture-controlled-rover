#include <WiFi.h>
#include <esp_now.h>
#include <Wire.h>

#define MPU6050_ADDR 0x68


// =====================================================
// RECEIVER MAC ADDRESS   ROVER MAC ADDRESS: 38:3E:51:58:B1:B0

// =====================================================

// CHANGE THIS TO YOUR ROVER ESP32 MAC ADDRESS

uint8_t receiverMacAddress[] = {
  0x38, 0x3E, 0x51, 0x58, 0xB1, 0xB0
};


// =====================================================
// MESSAGE
// =====================================================

typedef struct {
  char command;
} Message;

Message message;


// =====================================================
// MPU6050
// =====================================================

void writeMPU6050(byte reg, byte value) {

  Wire.beginTransmission(MPU6050_ADDR);

  Wire.write(reg);
  Wire.write(value);

  Wire.endTransmission();
}


void readMPU6050(
  int16_t &AcX,
  int16_t &AcY,
  int16_t &AcZ
) {

  Wire.beginTransmission(MPU6050_ADDR);

  Wire.write(0x3B);

  Wire.endTransmission(false);

  Wire.requestFrom(MPU6050_ADDR, 6, true);

  AcX = Wire.read() << 8 | Wire.read();

  AcY = Wire.read() << 8 | Wire.read();

  AcZ = Wire.read() << 8 | Wire.read();
}


// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);

  // MPU6050
  Wire.begin(21, 22);

  // Wake MPU6050
  writeMPU6050(0x6B, 0x00);

  delay(100);

  Serial.println("MPU6050 Started");


  // ESP-NOW
  WiFi.mode(WIFI_STA);

  delay(100);

  Serial.print("GLOVE MAC ADDRESS: ");
  Serial.println(WiFi.macAddress());


  if (esp_now_init() != ESP_OK) {

    Serial.println("ESP-NOW initialization failed!");

    return;
  }


  esp_now_peer_info_t peerInfo = {};

  memcpy(
    peerInfo.peer_addr,
    receiverMacAddress,
    6
  );

  peerInfo.channel = 0;

  peerInfo.encrypt = false;


  if (esp_now_add_peer(&peerInfo) != ESP_OK) {

    Serial.println("Failed to add receiver!");

    return;
  }


  Serial.println("Transmitter ready!");
}


// =====================================================
// LOOP
// =====================================================

void loop() {

  int16_t AcX;
  int16_t AcY;
  int16_t AcZ;


  // Read MPU6050
  readMPU6050(AcX, AcY, AcZ);


  // Convert to g
  float x = AcX / 16384.0;

  float y = AcY / 16384.0;

  float z = AcZ / 16384.0;


  // ===================================================
  // GESTURE DETECTION
  // ===================================================

  if (y > 0.35) {

    message.command = 'F';

    Serial.print("X = ");
    Serial.print(x, 2);

    Serial.print("  Y = ");
    Serial.print(y, 2);

    Serial.print("  Z = ");
    Serial.print(z, 2);

    Serial.println("  ---> FORWARD");
  }


  else if (y < -0.35) {

    message.command = 'B';

    Serial.print("X = ");
    Serial.print(x, 2);

    Serial.print("  Y = ");
    Serial.print(y, 2);

    Serial.print("  Z = ");
    Serial.print(z, 2);

    Serial.println("  ---> BACKWARD");
  }


  else if (x > 0.35) {

    message.command = 'R';

    Serial.print("X = ");
    Serial.print(x, 2);

    Serial.print("  Y = ");
    Serial.print(y, 2);

    Serial.print("  Z = ");
    Serial.print(z, 2);

    Serial.println("  ---> RIGHT");
  }


  else if (x < -0.35) {

    message.command = 'L';

    Serial.print("X = ");
    Serial.print(x, 2);

    Serial.print("  Y = ");
    Serial.print(y, 2);

    Serial.print("  Z = ");
    Serial.print(z, 2);

    Serial.println("  ---> LEFT");
  }


  else {

    message.command = 'S';

    Serial.print("X = ");
    Serial.print(x, 2);

    Serial.print("  Y = ");
    Serial.print(y, 2);

    Serial.print("  Z = ");
    Serial.print(z, 2);

    Serial.println("  ---> STOP");
  }


  // ===================================================
  // SEND COMMAND
  // ===================================================

  esp_err_t result = esp_now_send(
    receiverMacAddress,
    (uint8_t *)&message,
    sizeof(message)
  );


  if (result != ESP_OK) {

    Serial.println("Send failed!");
  }


  delay(200);
}
