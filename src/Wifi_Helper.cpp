#ifdef WITH_WIFI
#include "Wifi_Helper.h"

using namespace Printer;
using namespace std;

namespace Wifi_Helper
{
    WiFiClient wifiClient;

    namespace
    {
        const int8_t readBufferMax = 64;
        vector<char> readBuffer;
        Enable wifiEnable = Enable::ENABLE_NONE;
        TaskThread taskUpdate;

#ifdef WOKWI
        String wifi_ssid = "Wokwi-GUEST";
        String wifi_password = "";
        String wifi_local_ip = "10.13.37.110";
        String wifi_server_ip = "10.13.37.2";
        int32_t wifi_server_port = 20240;
        String wifi_teleplot_ip = "host.wokwi.internal";
        int32_t wifi_teleplot_port = 47269;
#else
        // TODO : Change the default password and SSID name
        // SSID name maximum of 63 characters;
        String wifi_ssid = "Mecapi";
        // The password must be at least 8 to 63 characters long
        String wifi_password = "Mecapi2025";
        String wifi_local_ip = "192.168.137.110";
        String wifi_server_ip = "192.168.137.1";
        String wifi_teleplot_ip = wifi_server_ip;
        int32_t wifi_server_port = 20240;
        int32_t wifi_teleplot_port = 47269;
#endif
        bool wifi_changed = false;

    }

    void EnableWifi(Enable enable)
    {
        if (enable != Enable::ENABLE_NONE)
        {
            Printer::print("Wifi : ");

            if (wifiEnable == enable && enable == Enable::ENABLE_TRUE)
            {
                Printer::print("already Enable");
            }
            if (wifiEnable == enable && enable == Enable::ENABLE_FALSE)
            {
                Printer::print("already Disable");
                wifiClient.stop();
                WiFi.disconnect();
            }
            if (wifiEnable != enable && enable == Enable::ENABLE_TRUE)
            {
                Printer::print(" Enable");
            }
            if (wifiEnable != enable && enable == Enable::ENABLE_FALSE)
            {
                Printer::print(" Disable");
                wifiClient.stop();
                WiFi.disconnect();
            }
            Printer::println();
            wifiEnable = enable;
        }
    }

    bool IsEnable() { return wifiEnable == Enable::ENABLE_TRUE; }

    void Initialisation()
    {
        wifiEnable = Enable::ENABLE_TRUE;
        Serial.println("-- Starting Wifi initialisation --");

        // delete old config only if wifi is connected, if not : does nothing
        WiFi.disconnect(true);

        WiFi.mode(WIFI_STA);

        Serial.print("AP MAC : ");
        Serial.println(WiFi.softAPmacAddress());
        Serial.print("Wifi MAC : ");
        Serial.println(WiFi.macAddress());

        // Get the Local Static IP saved
        String saved_wifi_local_ip = Preferences_Helper::LoadFromPreference("wifi_local_ip", wifi_local_ip);
        if (saved_wifi_local_ip != wifi_local_ip)
        {
            wifi_local_ip = saved_wifi_local_ip;
            Serial.println("Wifi local ip saved : " + wifi_local_ip);
        }
        else
            Serial.println("No wifi local ip saved, using default : " + wifi_local_ip);

        // Set the Local Static IP address
        IPAddress local_IP;
        local_IP.fromString(wifi_local_ip);

        // Get the Server Static IP saved
        String saved_wifi_server_ip = Preferences_Helper::LoadFromPreference("wifi_server_ip", wifi_server_ip);
        if (saved_wifi_server_ip != wifi_server_ip)
        {
            wifi_server_ip = saved_wifi_server_ip;
            Serial.println("Wifi server ip saved : " + wifi_server_ip);
        }
        else
            Serial.println("No wifi server ip saved, using default : " + wifi_server_ip);

        // Set the Server Static IP address
        IPAddress server_IP;
        server_IP.fromString(wifi_server_ip);

        // Get the Server Port saved
        int32_t saved_wifi_server_port = Preferences_Helper::LoadFromPreference("wifi_server_port", wifi_server_port);
        if (saved_wifi_server_port != wifi_server_port)
        {
            wifi_server_port = saved_wifi_server_port;
            Serial.println("Wifi server port saved : " + String(wifi_server_port));
        }
        else
            Serial.println("No wifi server port saved, using default : " + String(wifi_server_port));

        IPAddress subnet(255, 255, 255, 0);
        // IPAddress primaryDNS(8, 8, 8, 8);   //optional
        // IPAddress secondaryDNS(8, 8, 4, 4); //optional

        // Configures static IP address
        if (!WiFi.config(local_IP, server_IP, subnet)) //, primaryDNS, secondaryDNS))
        {
            Serial.println("STA Failed to configure");
        }

        String saved_wifi_password = Preferences_Helper::LoadFromPreference("wifi_password", wifi_password);
        if (saved_wifi_password != wifi_password)
        {
            wifi_password = saved_wifi_password;
            Serial.println("Wifi server ip saved : " + wifi_password);
        }
        else
            Serial.println("No wifi password saved, using default : " + wifi_password);

        String saved_wifi_ssid = Preferences_Helper::LoadFromPreference("wifi_ssid", wifi_ssid);
        if (saved_wifi_ssid != wifi_ssid)
        {
            wifi_ssid = saved_wifi_ssid;
            Serial.println("Wifi ssid saved : " + wifi_ssid);
        }
        else
            Serial.println("No wifi ssid saved, using default : " + wifi_ssid);

        // Events callback (to reconnect)
        WiFi.onEvent(WiFiStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
        WiFi.onEvent(WiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

        // Begin WiFi
        WiFi.begin(wifi_ssid, wifi_password);

        //! Connection with wifiClient to server will be done later in the Update task

        readBuffer.reserve(readBufferMax);
        readBuffer.clear();

        Serial.println("Creating Wifi Update Task");
        /* Task function. */
        /* name of task. */
        /* Stack size of task */
        /* parameter of the task */
        /* priority of the task */
        /* Task handle to keep track of created task */
        /* pin task to core 0 */
        taskUpdate = TaskThread(Wifi_Helper::Update, "WifiUpdate", 10000, 10, 0);

#ifdef WITH_OTA
        Printer::println("Initialisation of OTA");
        ArduinoOTA
            .onStart([]()
                     {
            String type;
            if (ArduinoOTA.getCommand() == U_FLASH) {
                type = "sketch";
            } else {  // U_SPIFFS
                type = "filesystem";
            }

            // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
            Printer::println("Start updating " + type); })
            .onEnd([]()
                   { Printer::println("\nEnd"); })
            .onProgress([](unsigned int progress, unsigned int total)
                        { Printer::println("Progress: ", (progress / (total / 100)), " %"); })
            .onError([](ota_error_t error)
                     { Printer::println("Error[", error,"]: ");
            if (error == OTA_AUTH_ERROR) {
                Printer::println("Auth Failed");
            } else if (error == OTA_BEGIN_ERROR) {
                Printer::println("Begin Failed");
            } else if (error == OTA_CONNECT_ERROR) {
                Printer::println("Connect Failed");
            } else if (error == OTA_RECEIVE_ERROR) {
                Printer::println("Receive Failed");
            } else if (error == OTA_END_ERROR) {
                Printer::println("End Failed");
            } });
        ArduinoOTA.begin();
#endif

        Serial.println("-- End of Wifi initialisation --");
    }

    void Update(void *pvParameters)
    {
        for (;;)
        {
            if (IsEnable())
            {
                if (WiFi.status() == WL_CONNECTED)
                {
                    // Try to connect client to server
                    HandleClientConnection();

                    // Try to initialise UDP teleplot
                    HandleTeleplotConnection();
                }
                else
                {
                    // if WiFi is down, try reconnecting
                    HandleWifiConnection();
                }

                if (wifiClient.connected())
                {
                    while (wifiClient.available() > 0)
                    {
                        ProcessIncomingChar(wifiClient.read());
                    }
                }
#ifdef WITH_OTA
                ArduinoOTA.handle();
#endif
            }
            vTaskDelay(1);
        }
        Serial.println("Wifi Update Task STOPPED !");
    }

    void ProcessIncomingChar(char c)
    {
        if (c == '\r')
            return; // Ignore Carriage Return
        if (readBuffer.size() < readBuffer.capacity())
        {
            readBuffer.push_back(c);
            if (c == '\n')
            {
                if (readBuffer.size() > 1)
                {
                    wifiClient.print("Received ");
                    wifiClient.print(readBuffer.size());
                    wifiClient.print(" : ");
                    wifiClient.write(readBuffer.data(), readBuffer.size());
                    wifiClient.println();
                    ESP32_Helper::BufferReadCommand(readBuffer);
                }
                readBuffer.clear();
            }
        }
        else
        {
            wifiClient.print("wifiClient Read Buffer Overflow : ");
            wifiClient.println(readBuffer.size());
            readBuffer.clear();
        }
    }

    void HandleWifiConnection()
    {
        static Timeout wifiTO;
        if (!wifiTO.isRunning)
            wifiTO.Start(5000);

        if (wifiTO.IsTimeOut())
        {
            if (wifi_changed)
            {
                wifi_changed = false;
                Serial.println("Connecting to new WiFi : " + wifi_ssid);
                Serial.println("With password : " + wifi_password);
                WiFi.disconnect();
                WiFi.begin(wifi_ssid, wifi_password);
            }
            else
            {
                Serial.println("Reconnecting to WiFi...");
                WiFi.reconnect();
            }
        }

        if (Printer::teleplotUDP.IsInitialized())
        {
            // We need to un-init in case of wifi lost
            Serial.println("Deleting Teleplot");
            Printer::teleplotUDP.~Teleplot();
        }
    }

    void HandleClientConnection()
    {
        static Timeout clientTo;
        if (!clientTo.isRunning)
            clientTo.Start(5000);

        if (!wifiClient.connected() && clientTo.IsTimeOut())
        {
            wifiClient.stop();
            if (wifiClient.connect(wifi_server_ip.c_str(), wifi_server_port))
            {
                Serial.println("Connected to server !");
            }
            else
            {
                Serial.println("Connection to server failed");
            }
        }
    }

    void HandleTeleplotConnection()
    {
        static Timeout teleplotTO;
        if (!teleplotTO.isRunning)
            teleplotTO.Start(5000);

        if (!Printer::teleplotUDP.IsInitialized() && teleplotTO.IsTimeOut())
        {
            Serial.println("Initialising Teleplot");
            Printer::teleplotUDP = Teleplot(wifi_teleplot_ip.c_str(), wifi_teleplot_port);
        }
    }

    void HandleCommand(Command cmdTmp)
    {

        if (cmdTmp.cmd == "WifiStatus")
        {
            Printer::println("Wifi status : ", (wl_status_t)WiFi.status());
            Printer::println("Wifi SSID : ", WiFi.SSID());
            Printer::println("Wifi SSID saved : ", wifi_ssid);
            Printer::println("Wifi Password saved : ", wifi_password);

            Printer::println("Wifi IP : ", WiFi.localIP().toString());
            Printer::println("Wifi MAC : ", WiFi.macAddress());
            Printer::println("Wifi RSSI : ", WiFi.RSSI());
            Printer::println("Wifi Local IP : ", wifi_local_ip);
            Printer::println("Wifi Server IP : ", wifi_server_ip);
            Printer::println("Wifi Server Port : ", wifi_server_port);
        }
        else if (cmdTmp.cmd == "WifiEnable")
        {
            // WifiEnable:0
            // WifiEnable:1
            Printer::println("Wifi status : ", WiFi.status());
            if (cmdTmp.size == 1)
                EnableWifi((Enable)cmdTmp.data[0]);
        }
        else if (cmdTmp.cmd == "WifiPassword")
        {
            // Password must be at least 8 char or empty
            // WifiPassword:********
            if (cmdTmp.dataStr == "" || (cmdTmp.dataStr.length() >= 8 && cmdTmp.dataStr.length() <= 63))
            {
                Preferences_Helper::SaveToPreference("wifi_password", cmdTmp.dataStr);
                wifi_password = cmdTmp.dataStr;
                wifi_changed = true;
                Printer::println("Wifi password Changed !");
            }
            else
                Printer::println("Wifi Password must be at least 8 characters and max 63 char !");
        }
        else if (cmdTmp.cmd == "WifiSsid")
        {
            if (cmdTmp.dataStr != "" && cmdTmp.dataStr.length() <= 63)
            {
                Preferences_Helper::SaveToPreference("wifi_ssid", cmdTmp.dataStr);
                wifi_ssid = cmdTmp.dataStr;
                wifi_changed = true;
                Printer::println("Wifi SSID Changed !");
            }
            else
                Printer::println("Wifi SSID is empty or is too long (max 63 char) !");
        }
        else if (cmdTmp.cmd == "WifiLocalIP" && cmdTmp.size == 4)
        {
            String ip = "" + String(cmdTmp.data[0]) + "." + String(cmdTmp.data[1]) + "." + String(cmdTmp.data[2]) + "." + String(cmdTmp.data[3]);
            SetLocalIP(ip);
            Preferences_Helper::SaveToPreference("wifi_local_ip", ip);
            Printer::println("Wifi local IP Changed !");
        }
        else if (cmdTmp.cmd == "WifiServerIP" && cmdTmp.size == 4)
        {
            String ip = "" + String(cmdTmp.data[0]) + "." + String(cmdTmp.data[1]) + "." + String(cmdTmp.data[2]) + "." + String(cmdTmp.data[3]);
            SetServerIP(ip);
            Preferences_Helper::SaveToPreference("wifi_server_ip", ip);
            Printer::println("Wifi server IP Changed !");
        }
        else if (cmdTmp.cmd == "WifiServerPort" && cmdTmp.size == 1)
        {
            Preferences_Helper::SaveToPreference("wifi_server_port", cmdTmp.data[0]);
            Printer::println("Wifi server Port Changed !");
        }
        else
        {
            Printer::println("Not a Wifi command !");
            PrintCommandHelp();
        }
    }

    void PrintCommandHelp()
    {
        Printer::println("Wifi Command Help :");
        Printer::println(" > WifiStatus");
        Printer::println("      Print the Wifi status");
        Printer::println(" > WifiEnable:[bool]");
        Printer::println("      1 Enable or 0 disable the wifi");
        Printer::println(" > WifiPassword:[string]");
        Printer::println("      Change the Wifi password (min 8, max 63 characters)");
        Printer::println(" > WifiSsid:[string]");
        Printer::println("      Change the Wifi SSID (max 63 characters)");
        Printer::println(" > WifiLocalIP:[int]:[int]:[int]:[int]");
        Printer::println("      Set the local IP of the ESP");
        Printer::println(" > WifiServerIP:[int]:[int]:[int]:[int]");
        Printer::println("      Set the server IP for the ESP to connect");
        Printer::println(" > WifiServerPort:[int]");
        Printer::println("      Set the server Port for the ESP to connect");
        Printer::println();
    }

    void SetLocalIP(String ip)
    {
        IPAddress local_IP;
        local_IP.fromString(ip);
        wifi_local_ip = ip;
        // Configures static IP address
        if (!WiFi.config(local_IP, WiFi.gatewayIP(), WiFi.subnetMask()))
        {
            Serial.println("STA Failed to configure");
        }
        wifi_changed = true;
    }

    void SetServerIP(String ip)
    {
        IPAddress server_IP;
        server_IP.fromString(ip);
        wifi_server_ip = ip;
        // Configures static IP address
        if (!WiFi.config(WiFi.localIP(), server_IP, WiFi.subnetMask()))
        {
            Serial.println("STA Failed to configure");
        }
        wifi_changed = true;
    }

    void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info)
    {
        Serial.println("Connected to SSID " + WiFi.SSID() + " with IP " + WiFi.localIP().toString() + " successfully!");
    }

    void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
    {
        Serial.print("WiFi lost connection : ");
        Serial.println((wifi_err_reason_t)info.wifi_sta_disconnected.reason);
    }
}
#endif
