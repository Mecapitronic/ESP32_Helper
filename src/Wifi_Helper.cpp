#ifdef WITH_WIFI
#include "Wifi_Helper.h"
namespace Wifi_Helper
{
    WiFiClient wifiClient;

    namespace
    {
        Enable wifiEnable = ENABLE_NONE;

        TaskHandle_t TaskUpdate;

        // TODO : Change the default password and SSID name
        String wifi_ssid = "Mecapitronic";
        String wifi_password = "Geoffroy";
        String wifi_local_ip = "192.168.137.110";
        String wifi_server_ip = "192.168.137.1";
        int wifi_server_port = 20240;

        // const String wifi_ssid = "Wokwi-GUEST";
        // const String wifi_password = "";
        // const String wifi_local_ip = "10.13.37.110";
        // const String wifi_server = "host.wokwi.internal";
        
    }

    void EnableWifi(Enable enable)
    {
        if (enable != ENABLE_NONE)
        {
            Printer::print("Wifi : ");

            if (wifiEnable == enable && enable == ENABLE_TRUE)
            {
                Printer::print("already Enable");
            }
            if (wifiEnable == enable && enable == ENABLE_FALSE)
            {
                Printer::print("already Disable");
            }
            if (wifiEnable != enable && enable == ENABLE_TRUE)
            {
                Printer::print(" Enable");
            }
            if (wifiEnable != enable && enable == ENABLE_FALSE)
            {
                Printer::print(" Disable");
            }
            Printer::println();
            wifiEnable = enable;
        }
    }

    bool IsEnable() { return wifiEnable == ENABLE_TRUE; }

    void Initialisation()
    {
        wifiEnable = ENABLE_TRUE;
        Serial.println("-- Starting Wifi initialisation --");

        // delete old config only if wifi is connected, if not : does nothing
        WiFi.disconnect(true);

        WiFi.mode(WIFI_STA);
        delay(500);

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
        int saved_wifi_server_port = Preferences_Helper::LoadFromPreference("wifi_server_port", wifi_server_port);
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

        // time to connect to wifi
        delay(3000);

        // Connect to server
        if (wifiClient.connect(wifi_server_ip.c_str(), wifi_server_port))
        {
            Serial.println("Connected to server !");
        }
        else
        {
            Serial.println("Connection to server failed");
        }

        Serial.println("Creating Wifi Update Task");
        /* Task function. */
        /* name of task. */
        /* Stack size of task */
        /* parameter of the task */
        /* priority of the task */
        /* Task handle to keep track of created task */
        /* pin task to core 0 */
        xTaskCreatePinnedToCore(Wifi_Helper::Update, "WifiUpdate", 10000, NULL, 10, &TaskUpdate, 0);

        Serial.println("-- End of Wifi initialisation --");
    }

    unsigned long previousMillisWifi = 0;
    unsigned long previousMillisServer = 0;
    unsigned long intervalWifi = 5000;
    unsigned long intervalServer = 5000;
    unsigned long currentMillisWifi = 0;
    unsigned long currentMillisServer = 0;

    void Update(void *pvParameters)
    {
        uint16_t indexBuffer = 0;
        const int readBufferMax = 64;
        char readBuffer[readBufferMax];

        for (;;)
        {
            if(IsEnable())
            {
                currentMillisWifi = millis();
                // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
                if ((WiFi.status() != WL_CONNECTED) && (currentMillisWifi - previousMillisWifi >= intervalWifi))
                {
                    if (false)
                    {
                        Serial.println("Connecting to new WiFi : " + wifi_ssid);
                        WiFi.disconnect();
                        WiFi.begin(wifi_ssid, wifi_password);
                    }
                    else
                    {
                        Serial.println("Reconnecting to WiFi...");
                        WiFi.reconnect();
                    }
                    previousMillisWifi = currentMillisWifi;
                }

                currentMillisServer = millis();
                if (WiFi.status() == WL_CONNECTED)
                {
                    if (!wifiClient.connected() && (currentMillisServer - previousMillisServer >= intervalServer))
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
                        previousMillisServer = currentMillisServer;
                    }
                }            

                if (wifiClient.connected())
                {
                    while (wifiClient.available() > 0)
                    {
                        char tmpChar = wifiClient.read();
                        if (indexBuffer < readBufferMax)
                        {
                            readBuffer[indexBuffer++] = tmpChar;
                            if (tmpChar == '\n')
                            {
                                wifiClient.print("Received : ");
                                wifiClient.write(readBuffer, indexBuffer);
                                // Read and extract Commands
                                ESP32_Helper::BufferReadCommand(readBuffer, indexBuffer);
                                indexBuffer = 0;                          
                            }
                        }
                        else
                        {
                            wifiClient.print("Read Buffer Overflow : ");
                            wifiClient.println(indexBuffer);
                            indexBuffer = 0;
                        }
                    }
                }
            }
            vTaskDelay(1);
        }
        Serial.println("Wifi Update Task STOPPED !");
    }

    void HandleCommand(Command cmdTmp)
    {
        if (cmdTmp.cmd == "WifiEnable")
        {
            // WifiEnable:1
            if (cmdTmp.size == 1)
                EnableWifi((Enable)cmdTmp.data[0]);
        }
        else if (cmdTmp.cmd == "WifiPassword")
        {
            // Password must be at least 8 char
            // WifiPassword:********
            // Wifi_Helper::ChangePassword(cmdTmp.data[0]);
        }
        else if (cmdTmp.cmd == "WifiSsid")
        {
        }
        else if (cmdTmp.cmd == "WifiLocalIP" && cmdTmp.size == 4)
        {
            String ip = "" + String(cmdTmp.data[0]) + "." + String(cmdTmp.data[1]) + "." + String(cmdTmp.data[2]) + "." + String(cmdTmp.data[3]);
            Preferences_Helper::SaveToPreference("wifi_local_ip", ip);
        }
        else if (cmdTmp.cmd == "WifiServerIP" && cmdTmp.size == 4)
        {
            String ip = "" + String(cmdTmp.data[0]) + "." + String(cmdTmp.data[1]) + "." + String(cmdTmp.data[2]) + "." + String(cmdTmp.data[3]);
            Preferences_Helper::SaveToPreference("wifi_server_ip", ip);
        }
        else if (cmdTmp.cmd == "WifiServerPort" && cmdTmp.size == 1)
        {
            Preferences_Helper::SaveToPreference("wifi_server_port", cmdTmp.data[0]);
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
        Printer::println(" > WifiLocalIP:[int]:[int]:[int]:[int]");
        Printer::println("      Set the local IP of the ESP");
        Printer::println(" > WifiServerIP:[int]:[int]:[int]:[int]");
        Printer::println("      Set the server IP for the ESP to connect");
        Printer::println(" > WifiServerPort:[int]");
        Printer::println("      Set the server Port for the ESP to connect");
        Printer::println();
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