#include <DFRobotDFPlayerMini.h>

HardwareSerial mp3Serial(1);
DFRobotDFPlayerMini dfPlayer;

void setup()
{
  Serial.begin(115200);

  mp3Serial.begin(9600, SERIAL_8N1, 18, 17);

  if (!dfPlayer.begin(mp3Serial))
  {
    Serial.println("FAIL");
    while(1);
  }

  Serial.println("OK");

  dfPlayer.volume(10);

  delay(2000);

  Serial.println("PLAYING");

  dfPlayer.play(4);

  delay(2000);

  Serial.print("Current=");
  Serial.println(dfPlayer.readCurrentFileNumber());

  Serial.print("State=");
  Serial.println(dfPlayer.readState());
}

void loop()
{
}