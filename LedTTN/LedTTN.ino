#include <TheThingsNetwork.h>
#define PWM 6

int pwm=0;

// Set your AppEUI and AppKey
const char *appEui = "70B3D57ED0010A3D";
const char *appKey = "C8E009F58F77B4BF2F789BC98FCF6BD3";

#define loraSerial Serial1
#define debugSerial Serial

// Replace REPLACE_ME with TTN_FP_EU868 or TTN_FP_US915
#define freqPlan TTN_FP_EU868

TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);



void setup()
{
  pinMode(PWM,OUTPUT);
  analogWrite(PWM,pwm);
  
  loraSerial.begin(57600);
  debugSerial.begin(9600);

  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000)
    ;

  // Set callback for incoming messages
  ttn.onMessage(message);

  debugSerial.println("-- STATUS");
  ttn.showStatus();

  debugSerial.println("-- JOIN");
  ttn.join(appEui, appKey);
}

void loop()
{
  debugSerial.println("-- LOOP");

  // Prepare payload of 1 byte to indicate LED status
  byte payload[1];
  byte led6 = (map(pwm,0,255,0,100) <<  1);
  payload[0] = digitalRead(LED_BUILTIN)+led6;
  
  // Send it off
  ttn.sendBytes(payload, sizeof(payload));

  delay(5000);
}

void message(const byte *payload, size_t length, port_t port)
{
  debugSerial.println("-- MESSAGE");
  byte led13 = payload[0] & B00000001;
  byte led6 = (payload[0] >> 1);
  pwm = map(led6,0,100,0,255);
  
  // Only handle messages of a single byte
  if (length != 1)
  {
    return;
  }
  if (!led13)
  {
    debugSerial.println("LED: off");
    digitalWrite(LED_BUILTIN, LOW);
  }
  else
  {
    debugSerial.println("LED: on");
    digitalWrite(LED_BUILTIN, HIGH);
  }

  analogWrite(PWM,pwm);
}
