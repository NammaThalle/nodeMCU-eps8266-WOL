
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <WakeOnLan.h>
#include <NTPClient.h>

#define LOVE_LIGHT D1

WiFiUDP UDP;
WakeOnLan WOL(UDP);
NTPClient timeClient(UDP);

const char* ssid     = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

/**
 * @brief
 * This function wakes up the PC named Vempire from the network
 * 
 * @param None
 * 
 * @return None
*/
void wakeVempire() {
  const char *MACAddress = "AB:CD:EF:GH:IJ:KL";
  const char *secureOn = "FE:DC:BA:98:76:54";

  // send Wake On Lan packet with the above MAC address. Default to port 9.
  WOL.sendMagicPacket(MACAddress); 

  // change the port number
  // WOL.sendMagicPacket(MACAddress, secureOn, 7); // Change the port number
}

/**
 * @brief
 * This Function is responsible for Establishing connection
 * to the Wifi network
 * 
 * @param None
 * 
 * @return None
*/
void setupWIFI()
{
  // repeat the packet three times with 100ms between. WARNING delay() is used between send packet function.
  WOL.setRepeat(3, 100); 

  // change the wifi to station mode
  WiFi.mode(WIFI_STA);

  // begin with the connection
  WiFi.begin(ssid, password);

  // attempting to connect to the WIFI
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // printing the the SSID and the IP address
  Serial.print("\nConnected to: ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // to calculate the broadcast address, otherwise 255.255.255.255 is used (which is denied in some networks).
  WOL.calculateBroadcastAddress(WiFi.localIP(), WiFi.subnetMask()); 
}

/**
 * @brief
 * This function is responsible to setup the Time and 
 * related activities
 * 
 * @param None
 * 
 * @return None
*/
void setupTime()
{
  // set to Your Time Zone: GMT +XX:XX
  timeClient.setTimeOffset(YOUR_OFFSET);

  // start the timer
  timeClient.begin();

  // fetch the latest time
  timeClient.update();

  // print the current time
  Serial.print("Current Time: ");
  Serial.println(timeClient.getFormattedTime());
}

/**
 * This function updates the Love Lights status i.e 
 * Turns them off or on, depending on the time.
 * If the time is 6:00pm or more the lights will turn on
 * If the time is 6:00am or less the lights will turn off
 * 
 * PS: Love lights is nothing but a series of LED lights 
 * which works on 3.3v cells 
 * 
 * @param 
 * int hours - Current Hour from time
 * int minutes - Current Minute from time
 * 
 * @return None
*/
void updateLoveLights(int hours, int minutes)
{
  if(hours >= 6 && hours < 18)
  { 
    // it's morning time
    digitalWrite(LOVE_LIGHT, LOW);
  }
  else 
  {
    // it's evening time
    digitalWrite(LOVE_LIGHT, HIGH);
  }
}

/**
 * @brief
 * This function performs all necessary setups required for this program
 * 
 * @param None
 * 
 * @return None
*/
void setup()
{   
  // configure the Digital pin where Love lights are connected to OUTPUT mode
  pinMode(LOVE_LIGHT, OUTPUT);

  // begin the serial communication at 115200 baudrate
  Serial.begin(115200);

  // setup the wifi with specified credentials
  setupWIFI();

  // setup Time Server and configure to India Time - GMT +05:30
  setupTime();

  // issue wake-on LAN signal to the ProxMox Server
  wakeVempire();
}

/**
 * @brief
 * This function is responsible for continuously running
 * some function of code
 * 
 * @param None
 *
 * @return None
*/
void loop()
{

  // update love lights depending on the current time of the day
  updateLoveLights(timeClient.getHours(), timeClient.getMinutes());

  // delay for 5 minutes
  delay(300000);

  // fetch the latest time
  timeClient.update();
}
