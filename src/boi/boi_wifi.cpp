#include "boi_wifi.h"
#include "app.h"
#include "WiFi.h"

boi_wifi *_globalBoiWifi;
pthread_mutex_t lock;
pthread_mutex_t lockDone;

void *static_monitor_captive_portal(void *)
{
    if(_globalBoiWifi)
        _globalBoiWifi->monitor_captive_portal();

    return 0;
}


boi_wifi::boi_wifi(boi *boiData, Messages *message_handler, WifiModeEnum NewMode)
{
    LEDHandler->StartScript(LED_ENABLE_WIFI, 1);
    this->_boi = boiData;
    this->message_handler = message_handler;
    this->ServerCheckThread = 0;
    Mode = NewMode;

    _globalBoiWifi = this;

    switch(NewMode)
    {
        case boi_wifi::BusinessCardMode:
            this->ActivateBusinessCard();
            return;

        case boi_wifi::PartyMode:
            this->ActivateParty();
            return;

        case boi_wifi::RickMode:
            this->ActivateRick();
            return;

        default:
            this->ActivateNormal();
            return;
    }
    
}

boi_wifi::~boi_wifi()
{
    this->DisableWiFi();

    //indicate wifi was off
    this->preferences.begin("options");
    this->preferences.putUChar("wifi", 0);
    this->preferences.end();
}

void boi_wifi::DisableWiFi()
{
    //kill the thread
    Serial.println("Disabling wifi");
    LEDHandler->StartScript(LED_DISABLE_WIFI, 1);

    pthread_mutex_lock(&lock);
    pthread_mutex_lock(&lockDone);
    pthread_mutex_destroy(&lock);
    pthread_mutex_destroy(&lockDone);

    Serial.println("Wifi disabled");

    //now turn off the dns and captive portal
    this->message_handler->DeregisterWebSocket();

    //remove and delete the request handler
    this->DeleteWebServer();
    this->dnsServer->stop();

    //delete the dns memory
    delete this->dnsServer;

    //wipe out the variables
    this->dnsServer = 0;
    this->server = 0;
    this->request_handler = 0;
    this->ServerCheckThread = 0;

    WiFi.enableAP(false);

    //remove our global
    _globalBoiWifi = 0;
}

void boi_wifi::monitor_captive_portal(){
    SensorDataStruct SensorData;
    int LoopCount;
    int LoopScanCount;

    pthread_mutex_lock(&lockDone);

    LoopCount = 0;
    LoopScanCount = 0;
    while(1)
    {
        if(pthread_mutex_trylock(&lock))
        {
            Serial.println("Exiting thread");
            break;
        }

        this->dnsServer->processNextRequest();
        yield();
        
        //increment our loop count
        LoopCount++;
        LoopScanCount++;
        if(LoopCount >= 5)
        {
            //read and send sensor data roughly every half a second
            //not exact due to timing and other activities on the device but this does not have
            //to be accurate
            this->_boi->get_sensor_data(&SensorData);
            yield();
            this->message_handler->HandleSensorData(&SensorData);
            yield();
            LoopCount = 0;
        }
        pthread_mutex_unlock(&lock);

        delay(100);
    };

    pthread_mutex_unlock(&lockDone);
}

void boi_wifi::monitor_smwn(){
    SensorDataStruct SensorData;
    int LoopCount;
    int LoopScanCount;

    pthread_mutex_lock(&lockDone);

    LoopCount = 0;
    LoopScanCount = 0;
    while(1)
    {
        if(pthread_mutex_trylock(&lock))
        {
            Serial.println("Exiting thread");
            break;
        }

        yield();
        
        //increment our loop count
        LoopCount++;
        LoopScanCount++;
        if(LoopCount >= 5)
        {
            //read and send sensor data roughly every half a second
            //not exact due to timing and other activities on the device but this does not have
            //to be accurate
            this->_boi->get_sensor_data(&SensorData);
            yield();
            this->message_handler->HandleSensorData(&SensorData);
            yield();
            // Monitor Itero for new messages
            

            LoopCount = 0;
        }
        pthread_mutex_unlock(&lock);

        delay(100);
    };

    pthread_mutex_unlock(&lockDone);
}

void boi_wifi::Reconfigure(const OptionsStruct *Options)
{
    char WifiName[20 + 1 + 10];
    // uint8_t BatterySymbol[] = " \xF0\x9F\x94\x8B ";
    uint8_t BatterySymbol[] = " \xF0\x9F\x8E\x83 "; // SPOOKY EDITION OOOOOOOOOoOOOoOooooooooo.............

    memcpy(WifiName, &BatterySymbol[1], 5);

    //reconfigure the wifi with batteries around it
    memcpy(&WifiName[5], Options->WifiName, strlen(Options->WifiName));
    memcpy(&WifiName[5 + strlen(Options->WifiName)], BatterySymbol, 5);
    WifiName[10 + strlen(Options->WifiName)] = 0;
    Serial.printf("Wifi portal name: %s\n", WifiName);

    if(!WiFi.softAP(WifiName, Options->WifiPassword))
        Serial.print("Error calling WiFi.softAP\n");
}

void boi_wifi::setup_captive_portal(const OptionsStruct *Options){
    const byte        DNS_PORT = 53;          // Capture DNS requests on port 53
    IPAddress         apIP(1, 1, 1, 1);    // Private network for server

    if(this->ServerCheckThread)
    {
        this->DisableWiFi();
        delay(1000);
        yield();
    }

    if(!WiFi.mode(WIFI_AP_STA))
        Serial.print("Error setting WiFi mode\n");

    this->Reconfigure(Options);

    if(!WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0)))
        Serial.print("Error calling WiFi.softAPConfig\n");

    // if DNSServer is started with "*" for domain name, it will reply with
    // provided IP to all DNS request
    this->dnsServer = new DNSServer();
    this->dnsServer->start(DNS_PORT, "*", apIP);

    // reply to all requests with same HTML
    this->SetupRequestServer();

    //setup the web socket
    this->message_handler->RegisterWebsocket();

    pthread_mutex_init(&lock, NULL);
    pthread_mutex_init(&lockDone, NULL);

    //setup our thread that will re-send messages that haven't been ack'data
    if(pthread_create(&this->ServerCheckThread, NULL, static_monitor_captive_portal, 0))
    {
        //failed
        Serial.println("Failed to setup ServerCheck thread");
        _globalBoiWifi = 0;
    }
}

void boi_wifi::enter_safe_mode_with_networking(const OptionsStruct *Options){
    //3 button presses to enter safe_mode_with_networking
    // Connection code to connect to an AP
    // No DNS
    // Initialize the webserver still
    // Initialize the mesh network

    if(this->ServerCheckThread)
    {
        this->DisableWiFi();
        delay(1000);
        yield();
    }

    // Set up Wifi Connection Here
    // Fetch local network name from options
    // Fetch local wifi password from options
    WiFi.begin("Asgard","notaplace!");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi Safe Mode with Networking...");
    }
 
    Serial.println("Connected to the WiFi for Safe Mode with Networking");

    // reply to all requests with same HTML
    this->SetupRequestServer();

    //setup the web socket
    this->message_handler->RegisterWebsocket();

    pthread_mutex_init(&lock, NULL);
    pthread_mutex_init(&lockDone, NULL);

    //setup our thread that will re-send messages that haven't been ack'data
    if(pthread_create(&this->ServerCheckThread, NULL, static_monitor_captive_portal, 0))
    {
        //failed
        Serial.println("Failed to setup ServerCheck thread");
        _globalBoiWifi = 0;
    }

    // Post out to batteryinter.net with our local IP address


}