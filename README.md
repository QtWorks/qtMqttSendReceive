# qtMqttSendReceive
How to control actuators and show their real states over MQTT protocol.

In this project, we control a binary actuator (simple LED) and an analogic actuator (PWM LED). Commands are published using mqtt protocol to The Things Network wher an application with a device is create.

To control this LEDs, command is simple byte where b0 control the simple LED and b7..b1 control the PWM LED.
The state of LEDs is sent by Arduino according the same protocol. A decoder function in TTN console do a json object like this :

```javascript
{
  led : 0 or 1,
  pwm : 0..100
}
```

MQTT from or to TTN is documented <a targer="blank" href="https://www.thethingsnetwork.org/docs/applications/mqtt/api.html">here</a>.

- Uplink :
```mqtt
my-app-id/devices/my-dev-id/up/<field>
```
- Downlink :
```mqtt
<AppID>/devices/<DevID>/down
payload : {"port":1,"payload_raw":"Base64 message"}
```

Note : to convert hex data to base 64 : http://tomeko.net/online_tools/hex_to_base64.php?lang=en

## 1. Console TTN :

- Create an application "leds control"
- In this application, create a device "leds-1"
- In Payload Formats tab, adjust the decoder function :

```javascript
function Decoder(bytes, port) {
  // Decode an uplink message from a buffer
  // (array) of bytes to an object of fields.
  var decoded = {};
  var led;
  var pwm;

  if (port === 1) 
  {
    led = bytes[0] & 1;
    pwm = bytes[0] >> 1;
    decoded.led = led;
    decoded.pwm = pwm+1;
  }

  return decoded;
}
```

## 2. Arduino :

- Compile and download the DeviceInfo.ino sketch from TheTingsNetwork library example.
- Get the DevEUI from the terminal and copy it to the device declaration in the TTN console.
- Connect a LED to pin PWM 6 and a LED to PIN 13 or use the builtin LED.
- Get the sketch LedTTN.ino and adjust the AppEUI and AppKey according yours Application and Device TTN console.
- Compile and download the sketch

## 3. Qt project

- Note you have to dispose the Qt MQTT module to run this project (<a target="blank" href="https://doc.qt.io/QtMQTT/index.html">https://doc.qt.io/QtMQTT/index.html</a>). <a target="blank" href="http://silanus.fr/sin/?p=1194">Installation tutorial</a>
- Get the project from git **testMqttLed**
- Adjust defines and MQTT client properties according your TTN configuration :
```C++
#define TOPIC_UP "YOUR_APP_NAME/devices/YOUR_DEVICE_NAME/up"
#define TOPIC_DOWN "YOUR_APP_NAME/devices/YOUR_DEVICE_NAME/down"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_client = new QMqttClient(this);
    m_client->setHostname("eu.thethings.network");
    m_client->setPort(1883);
    m_client->setUsername(YOUR_APP_NAME);
    m_client->setPassword(YOUR_APP_ACCESS_KEY);

    ....
}
```
- Compile and execute
<p align="center">
<img src="http://silanus.fr/sin/wp-content/uploads/2018/07/qtTestMqttLed.png">
</p>


