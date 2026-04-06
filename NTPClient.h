#pragma once

#include "Arduino.h"

#include <Udp.h>

#define SEVENZYYEARS 2208988800UL
#define NTP_PACKET_SIZE 48
#define NTP_DEFAULT_LOCAL_PORT 1337

class NTPClient {
private:
  UDP* _udp;
  bool _udpSetup = false;

  const char* _poolServerName = "pool.ntp.org";  // Default time server
  IPAddress _poolServerIP;
  unsigned int _port = NTP_DEFAULT_LOCAL_PORT;
  long _timeOffset = 0;

  unsigned long _currentEpoc = 0;  // In s
  unsigned long _lastUpdate = 0;   // In ms

  byte _packetBuffer[NTP_PACKET_SIZE];

  void sendNTPPacket();

public:
  NTPClient(UDP& udp);
  NTPClient(UDP& udp, long timeOffset);
  NTPClient(UDP& udp, const char* poolServerName);
  NTPClient(UDP& udp, const char* poolServerName, long timeOffset);
  NTPClient(UDP& udp, IPAddress poolServerIP);
  NTPClient(UDP& udp, IPAddress poolServerIP, long timeOffset);

  /**
     * Set time server name
     *
     * @param poolServerName
     */
  void setPoolServerName(const char* poolServerName);

  /**
     * Set random local port
     */
  void setRandomPort(unsigned int minValue = 49152, unsigned int maxValue = 65535);

  /**
     * Starts the underlying UDP client with the default local port
     */
  void begin();

  /**
     * Starts the underlying UDP client with the specified local port
     */
  void begin(unsigned int port);

  /**
     * This will force the update from the NTP Server.
     *
     * @return true on success, false on failure
     */
  bool forceUpdate();

  /**
     * This allows to check if the NTPClient successfully received a NTP packet and set the time.
     *
     * @return true if time has been set, else false
     */
  bool isTimeSet() const;

  int getDay() const;
  int getHours() const;
  int getMinutes() const;
  int getSeconds() const;

  /**
     * Changes the time offset. Useful for changing timezones dynamically
     */
  void setTimeOffset(int timeOffset);

  /**
     * @return time formatted like `hh:mm:ss`
     */
  String getFormattedTime() const;

  /**
     * @return time in seconds since Jan. 1, 1970
     */
  unsigned long getEpochTime() const;

  /**
     * Stops the underlying UDP client
     */
  void end();
};
