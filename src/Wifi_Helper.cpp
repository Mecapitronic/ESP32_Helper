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
        bool OTARunning;

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
        String wifi_base_ip = "192.168.43"; // tel
        String wifi_local_ip = "192.168.43.110"; // Local IP will be 192.168.43.(100 + numPami), default is 110 for Robot
        String wifi_server_ip = "192.168.43.215";
        //String wifi_base_ip = "192.168.137"; // PC
        //String wifi_local_ip = "192.168.137.110"; // Local IP will be 192.168.137.(100 + numPami), default is 110 for Robot
        //String wifi_server_ip = "192.168.137.1";
        String wifi_teleplot_ip = wifi_server_ip;
        int32_t wifi_server_port = 20240;
        int32_t wifi_teleplot_port = 47269;
#endif
        bool wifi_changed = false;

    }

    bool IsOTARunning() { return OTARunning; }

    void EnableWifi(Enable enable)
    {
        if (enable != Enable::ENABLE_NONE)
        {
            const char *status = "unknown";

            if (wifiEnable == enable && enable == Enable::ENABLE_TRUE)
            {
                status = "already Enable";
            }
            if (wifiEnable == enable && enable == Enable::ENABLE_FALSE)
            {
                status = "already Disable";
            }
            if (wifiEnable != enable && enable == Enable::ENABLE_TRUE)
            {
                status = "Enable";
            }
            if (wifiEnable != enable && enable == Enable::ENABLE_FALSE)
            {
                status = "Disable";
            }
            Printer::println("Wifi %s", status);
            wifiEnable = enable;
        }
    }

    bool IsEnable() { return wifiEnable == Enable::ENABLE_TRUE; }

    bool IsWifiConnected() { return WiFi.status() == WL_CONNECTED; }

    bool IsClientConnected() { return wifiClient.connected(); }

    void Initialisation()
    {
        wifiEnable = Enable::ENABLE_TRUE;
        println("-- Starting Wifi initialisation --");

        // delete old config only if wifi is connected, if not does nothing
        WiFi.disconnect(true);

        WiFi.mode(WIFI_STA);

        println("AP MAC %s", WiFi.softAPmacAddress().c_str());
        println("Wifi MAC %s", WiFi.macAddress().c_str());

        // Get the Local Static IP saved
        String saved_wifi_local_ip = Preferences_Helper::LoadFromPreference("wifi_local_ip", "");
        if (saved_wifi_local_ip != "")
        {
            wifi_local_ip = saved_wifi_local_ip;
            println("Wifi local ip saved " + wifi_local_ip);
        }
        else
            println("No wifi local ip saved, using default " + wifi_local_ip);

        // Set the Local Static IP address
        IPAddress local_IP;
        local_IP.fromString(wifi_local_ip);

        // Get the Server Static IP saved
        String saved_wifi_server_ip = Preferences_Helper::LoadFromPreference("wifi_server_ip", "");
        if (saved_wifi_server_ip != "")
        {
            wifi_server_ip = saved_wifi_server_ip;
            println("Wifi server ip saved " + wifi_server_ip);
        }
        else
            println("No wifi server ip saved, using default " + wifi_server_ip);

        // Set the Server Static IP address
        IPAddress server_IP;
        server_IP.fromString(wifi_server_ip);

        // Get the Teleplot Static IP saved
        String saved_wifi_teleplot_ip = Preferences_Helper::LoadFromPreference("wifi_tlp_ip", "");
        if (saved_wifi_teleplot_ip != "")
        {
            wifi_teleplot_ip = saved_wifi_teleplot_ip;
            println("Wifi teleplot ip saved " + wifi_teleplot_ip);
        }
        else
            println("No wifi teleplot ip saved, using default " + wifi_teleplot_ip);

        // Get the Server Port saved
        int32_t saved_wifi_server_port = Preferences_Helper::LoadFromPreference("wifi_server_port", -1);
        if (saved_wifi_server_port != -1)
        {
            wifi_server_port = saved_wifi_server_port;
            println("Wifi server port saved " + String(wifi_server_port));
        }
        else
            println("No wifi server port saved, using default " + String(wifi_server_port));

        IPAddress subnet(255, 255, 255, 0);
        // IPAddress primaryDNS(8, 8, 8, 8);   //optional
        // IPAddress secondaryDNS(8, 8, 4, 4); //optional

        // Configures static IP address
        if (!WiFi.config(local_IP, server_IP, subnet)) //, primaryDNS, secondaryDNS))
        {
            println("STA Failed to configure");
        }

        String saved_wifi_password = Preferences_Helper::LoadFromPreference("wifi_password", "");
        if (saved_wifi_password != "")
        {
            wifi_password = saved_wifi_password;
            println("Wifi password saved " + wifi_password);
        }
        else
            println("No wifi password saved, using default " + wifi_password);

        String saved_wifi_ssid = Preferences_Helper::LoadFromPreference("wifi_ssid", "");
        if (saved_wifi_ssid != "")
        {
            wifi_ssid = saved_wifi_ssid;
            println("Wifi ssid saved " + wifi_ssid);
        }
        else
            println("No wifi ssid saved, using default " + wifi_ssid);

        // Events callback (to reconnect)
        WiFi.onEvent(WiFiStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
        WiFi.onEvent(WiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

        // Begin WiFi
        WiFi.begin(wifi_ssid, wifi_password);

        //! Connection with wifiClient to server will be done later in the Update task

        readBuffer.reserve(readBufferMax);
        readBuffer.clear();

        println("Creating Wifi Update Task");
        /* Task function. */
        /* name of task. */
        /* Stack size of task */
        /* parameter of the task */
        /* priority of the task */
        /* Task handle to keep track of created task */
        /* pin task to core 0 */
        taskUpdate = TaskThread(Wifi_Helper::Update, "WifiUpdate", 10000, 10, 0);

#ifdef WITH_OTA
        OTARunning = false;
        Printer::println("Initialisation of OTA");
        ArduinoOTA
            .onStart(
                []()
                {
                    String type;
                    if (ArduinoOTA.getCommand() == U_FLASH)
                    {
                        type = "sketch";
                    }
                    else
                    {  // U_SPIFFS
                        type = "filesystem";
                    }
                    Wifi_Helper::OTARunning = true;
                    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
                    Printer::println("Start updating " + type);
                })
            .onEnd([]() { Printer::println("\nEnd"); })
            .onProgress(
                [](unsigned int progress, unsigned int total)
                {
                    Printer::println("Progress %i %%", (progress / (total / 100)));
                    Wifi_Helper::OTARunning = true;
                })
            .onError(
                [](ota_error_t error)
                {
                    Printer::println("Error[" + String(error) + "]: ");
                    if (error == OTA_AUTH_ERROR)
                    {
                        Printer::println("Auth Failed");
                    }
                    else if (error == OTA_BEGIN_ERROR)
                    {
                        Printer::println("Begin Failed");
                    }
                    else if (error == OTA_CONNECT_ERROR)
                    {
                        Printer::println("Connect Failed");
                    }
                    else if (error == OTA_RECEIVE_ERROR)
                    {
                        Printer::println("Receive Failed");
                    }
                    else if (error == OTA_END_ERROR)
                    {
                        Printer::println("End Failed");
                    }
                });
        ArduinoOTA.begin();
#endif
        RegisterCommands();

        println("-- End of Wifi initialisation --");
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
                if (WiFi.status() != WL_CONNECTED || wifi_changed)
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
                    if (FileSystem_Helper::WriteASCII::timeout.IsTimeOut())
                    {
                        Printer::println("WiFi ASCII session timed out for file: %s with %u/%u bytes", FileSystem_Helper::WriteASCII::fileName.c_str(), static_cast<unsigned int>(FileSystem_Helper::WriteASCII::buffer.size()), static_cast<unsigned int>(FileSystem_Helper::WriteASCII::bufferSize));
                        FileSystem_Helper::WriteASCII::Reset();
                    }
                }
#ifdef WITH_OTA
                ArduinoOTA.handle();
#endif
            }
            else
            {
                if (wifiClient.connected())
                {
                    wifiClient.stop();
                }
                if (WiFi.status() == WL_CONNECTED)
                {
                    WiFi.disconnect();
                }
            }
            vTaskDelay(100);
        }
        println("Wifi Update Task STOPPED !");
    }

    void ProcessIncomingChar(char c)
    {
        if (FileSystem_Helper::WriteASCII::timeout.IsRunning())
        {
            FileSystem_Helper::WriteASCII::buffer.push_back(static_cast<uint8_t>(c));
            // Timeout will be restarted with each incoming character
            FileSystem_Helper::WriteASCII::timeout.Start();
            if (FileSystem_Helper::WriteASCII::buffer.size() >= FileSystem_Helper::WriteASCII::bufferSize)
            {
                FileSystem_Helper::WriteASCII::timeout.Stop();
                Printer::println("Processing complete WiFi ASCII session for file: %s with %u bytes", FileSystem_Helper::WriteASCII::fileName.c_str(), static_cast<unsigned int>(FileSystem_Helper::WriteASCII::buffer.size()));               
                if (FileSystem_Helper::AppendFile(FileSystem_Helper::WriteASCII::fileName, FileSystem_Helper::WriteASCII::buffer, true))
                {
                    Printer::println("Successfully appended to file: %s", FileSystem_Helper::WriteASCII::fileName.c_str());
                }
                else
                {
                    Printer::println("Failed to append to file: %s", FileSystem_Helper::WriteASCII::fileName.c_str());
                }
                FileSystem_Helper::WriteASCII::Reset();
            }
            return;
        }

        if (c == '\r')
            return; // Ignore Carriage Return
        if (readBuffer.size() < readBuffer.capacity())
        {
            readBuffer.push_back(c);
            if (c == '\n' || c == ';')
            {
                if (readBuffer.size() > 1)
                {
                    wifiClient.print("Received ");
                    wifiClient.print(readBuffer.size());
                    wifiClient.print(" ");
                    wifiClient.write(readBuffer.data(), readBuffer.size());
                    wifiClient.println();
                    Command command = ESP32_Helper::ParseCommandBuffer(readBuffer);
                    ESP32_Helper::HandleCommand(command);
                }
                readBuffer.clear();
            }
        }
        else
        {
            wifiClient.print("wifiClient Read Buffer Overflow ");
            wifiClient.println(readBuffer.size());
            readBuffer.clear();
        }
    }

    void HandleWifiConnection()
    {
        static Timeout wifiTO;
        if (!wifiTO.IsRunning())
            wifiTO.Start(5000);

        if (wifiTO.IsTimeOut())
        {
            if (wifi_changed)
            {
                wifi_changed = false;
                println("Connecting to new WiFi " + wifi_ssid);
                println("With password " + wifi_password);
                wifiClient.stop();
                WiFi.disconnect();
                WiFi.begin(wifi_ssid, wifi_password);
            }
            else
            {
                println("Reconnecting to WiFi...");
                WiFi.reconnect();
            }
        }

        if (Printer::teleplotUDP.IsInitialized())
        {
            // We need to un-init in case of wifi lost
            println("Deleting Teleplot because WiFi is down");
            Printer::teleplotUDP.Reset();
            println("Teleplot reset complete after WiFi loss");
        }
    }

    void HandleClientConnection()
    {
        static Timeout clientTo;
        if (!clientTo.IsRunning())
            clientTo.Start(5000);

        if (!wifiClient.connected() && clientTo.IsTimeOut())
        {
            wifiClient.stop();
            if (wifiClient.connect(wifi_server_ip.c_str(), wifi_server_port))
            {
                println("Connected to server !");
            }
            else
            {
                println("Connection to server failed");
            }
        }
    }

    void HandleTeleplotConnection()
    {
        static Timeout teleplotTO;
        if (!teleplotTO.IsRunning())
            teleplotTO.Start(5000);

        if (!Printer::teleplotUDP.IsInitialized() && teleplotTO.IsTimeOut() && Printer::teleplotUDPEnable == Enable::ENABLE_TRUE)
        {
            println("Initialising Teleplot on %s %i", wifi_teleplot_ip.c_str(), wifi_teleplot_port);
            Printer::teleplotUDP = Teleplot(wifi_teleplot_ip.c_str(), wifi_teleplot_port);
            println("Teleplot init state %s", Printer::teleplotUDP.IsInitialized() ? "ready" : "not ready");
        }

        if (Printer::teleplotUDP.IsInitialized() && Printer::teleplotUDPEnable == Enable::ENABLE_FALSE)
        {
            println("Resetting Teleplot because Teleplot UDP was disabled");
            Printer::teleplotUDP.Reset();
            println("Teleplot reset complete after disable request");
        }
    }
    
    void RegisterCommands()
    {        
        ESP32_Helper::RegisterCommand("WifiStatus", "WifiStatus - Show Wi-Fi status.", FunctionCommand( PrintWifiStatus(); ));
        ESP32_Helper::RegisterCommand("WifiEnable", "WifiEnable:<enabled:bool> - Enable or disable Wi-Fi.", FunctionCommand( if(cmd.hasData()) EnableWifi((Enable)cmd.data[0]); ));
        ESP32_Helper::RegisterCommand("WifiPassword", "WifiPassword:<password:string> - Change Wi-Fi password; 8..63 characters, within command buffer limits.", FunctionCommand( if(cmd.hasDataStr1()) SetPassword(String(cmd.dataStr1)); ));
        ESP32_Helper::RegisterCommand("WifiSsid", "WifiSsid:<ssid:string> - Change Wi-Fi SSID; 8..63 characters, within command buffer limits.", FunctionCommand( if(cmd.hasDataStr1()) SetSsid(String(cmd.dataStr1)); ));
        ESP32_Helper::RegisterCommand("WifiBaseIP", "WifiBaseIP:<octet1:int>:<octet2:int>:<octet3:int> - Set the IP prefix; each octet: 0..255. Example: WifiBaseIP:192:168:43", FunctionCommand( if(cmd.hasData(3)) SetBaseIP(cmd.data[0], cmd.data[1], cmd.data[2]); ));
        ESP32_Helper::RegisterCommand("WifiLocalIP", "WifiLocalIP:<octet:int> - Set the last octet of the local IP; octet: 0..255.", FunctionCommand( if(cmd.hasData()) SetLocalIP(cmd.data[0]); ));
        ESP32_Helper::RegisterCommand("WifiServerIP", "WifiServerIP:<octet:int> - Set the last octet of the server IP; octet: 0..255.", FunctionCommand( if(cmd.hasData()) SetServerIP(cmd.data[0]); ));
        ESP32_Helper::RegisterCommand("WifiTeleplotIP", "WifiTeleplotIP:<octet:int> - Set the last octet of the Teleplot IP; octet: 0..255.", FunctionCommand( if(cmd.hasData()) SetTeleplotIP(cmd.data[0]); ));
        ESP32_Helper::RegisterCommand("WifiServerPort", "WifiServerPort:<port:int> - Set the server TCP port; port: 1..65535.", FunctionCommand( if(cmd.hasData()) SetServerPort(cmd.data[0]); ));
    }

    void SetSsid(const String &ssid)
    {
        if(ssid == wifi_ssid)
        {
            println("SSID is the same as the current one, no change needed");
            return;
        }
        else if(ssid.length() < 8 || ssid.length() > 63)
        {
            println("SSID must be between 8 and 63 characters");
            return;
        }
        wifi_ssid = ssid;
        Preferences_Helper::SaveToPreference("wifi_ssid", ssid);
        wifi_changed = true;
    }

    void SetPassword(const String &password)
    {
        if(password == wifi_password)
        {
            println("Password is the same as the current one, no change needed");
            return;
        }
        else if(password.length() < 8 || password.length() > 63)
        {
            println("Password must be between 8 and 63 characters");
            return;
        }
        wifi_password = password;
        Preferences_Helper::SaveToPreference("wifi_password", password);
        wifi_changed = true;
    }

    void SetBaseIP(int first, int second, int third)
    {
        wifi_base_ip = String(first) + "." + String(second) + "." + String(third);
    }

    void SetLocalIP(const String &ip)
    {
        if(ip == wifi_local_ip)
        {
            println("Local IP is the same as the current one, no change needed");
            return;
        }
        IPAddress local_IP;
        local_IP.fromString(ip);
        wifi_local_ip = ip;
        // Configures static IP address
        if (!WiFi.config(local_IP, WiFi.gatewayIP(), WiFi.subnetMask()))
        {
            println("STA Failed to configure");
        }
        Preferences_Helper::SaveToPreference("wifi_local_ip", ip);
        wifi_changed = true;
    }

    void SetLocalIP(int lastDigit)
    {
        SetLocalIP(wifi_base_ip + "." + String(lastDigit));
    }

    void SetServerIP(const String &ip)
    {
        if(ip == wifi_server_ip)
        {
            println("Server IP is the same as the current one, no change needed");
            return;
        }
        IPAddress server_IP;
        server_IP.fromString(ip);
        wifi_server_ip = ip;
        // Configures static IP address
        if (!WiFi.config(WiFi.localIP(), server_IP, WiFi.subnetMask()))
        {
            println("STA Failed to configure");
        }
        Preferences_Helper::SaveToPreference("wifi_server_ip", ip);
        wifi_changed = true;
    }

    void SetServerIP(int lastDigit)
    {
        SetServerIP(wifi_base_ip + "." + String(lastDigit));
    }

    void SetTeleplotIP(const String &ip)
    {
        if(ip == wifi_teleplot_ip)
        {
            println("Teleplot IP is the same as the current one, no change needed");
            return;
        }
        wifi_teleplot_ip = ip;
        Preferences_Helper::SaveToPreference("wifi_tlp_ip", ip);
        Printer::teleplotUDP.Reset();
    }

    void SetTeleplotIP(int lastDigit)
    {
        SetTeleplotIP(wifi_base_ip + "." + String(lastDigit));
    }

    void SetServerPort(int port)
    {
        if(port == wifi_server_port)
        {
            println("Server port is the same as the current one, no change needed");
            return;
        }
        else if(port <= 0 || port > 65535)
        {
            println("Invalid server port");
            return;
        }
        wifi_server_port = port;
        Preferences_Helper::SaveToPreference("wifi_server_port", port);
        wifi_changed = true;
    }

    void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info)
    {
        println("Connected to SSID " + WiFi.SSID() + " with IP " + WiFi.localIP().toString() + " successfully!");
    }

    void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
    {
        println("WiFi lost connection " + String((wifi_err_reason_t)info.wifi_sta_disconnected.reason));
    }

    void PrintWifiStatus()
    {
        Printer::println("Wifi status %i", (wl_status_t)WiFi.status());
        Printer::println("Wifi SSID %s", WiFi.SSID().c_str());
        Printer::println("Wifi SSID saved %s", wifi_ssid.c_str());
        Printer::println("Wifi Password saved %s", wifi_password.c_str());

        Printer::println("Wifi IP %s", WiFi.localIP().toString().c_str());
        Printer::println("Wifi MAC %s", WiFi.macAddress().c_str());
        Printer::println("Wifi RSSI %i", WiFi.RSSI());
        Printer::println("Wifi Local IP %s", wifi_local_ip.c_str());
        Printer::println("Wifi Server IP %s", wifi_server_ip.c_str());
        Printer::println("Wifi Teleplot IP %s", wifi_teleplot_ip.c_str());
        Printer::println("Wifi Server Port %i", wifi_server_port);
    }
}
#endif
