#include "boi_wifi.h"
#include "esp_http_client.h"

void send_post_to_battery_internet(const char *message, unsigned int length){
    const char* uri = "https://batteryinter.net";
    
    const char* CAcert = "-----BEGIN CERTIFICATE-----"\
"MIIFPjCCBCagAwIBAgIQJ3fuKjxlyakKAAAAAEuOjDANBgkqhkiG9w0BAQsFADBC"\
"MQswCQYDVQQGEwJVUzEeMBwGA1UEChMVR29vZ2xlIFRydXN0IFNlcnZpY2VzMRMw"\
"EQYDVQQDEwpHVFMgQ0EgMUQyMB4XDTIwMDcxODIyMzUzOFoXDTIwMTAxNjIyMzUz"\
"OFowGzEZMBcGA1UEAxMQYmF0dGVyeWludGVyLm5ldDCCASIwDQYJKoZIhvcNAQEB"\
"BQADggEPADCCAQoCggEBAOd0BL+QP5AKLJyVFaH4ldRTUQy3qiwIgnpT1Nnwj0/p"\
"kIkBYI/O7sTB3abi4QMc41zYWRDJXjwWXn52m9XUjAE1PP9ZOY/yS275f068ki5r"\
"1YMPJEzbFrzHJbqhp6QCN1S1kpf+jtlaXmvqSmqY6clzVKcCnqQdkD9xMIVwvVYa"\
"mPSiJIud01PSn5EfZOQdIrEWpgziL25n/yuzpg/xKefUvEi4CkDruvDpLEb7/bUs"\
"mB7uLoFOq3IcZ61cBbxPa4h+vuc4QnBafq3PkVQfEGs843zN+KS4THLb5rQ+1NKZ"\
"YVjDQopkJcinYnHBZRwq3z5twLt0esQuUkEEHS3z71MCAwEAAaOCAlUwggJRMA4G"\
"A1UdDwEB/wQEAwIFoDATBgNVHSUEDDAKBggrBgEFBQcDATAMBgNVHRMBAf8EAjAA"\
"MB0GA1UdDgQWBBRbbdKK49/kCSNCxGodAebhWYUl0zAfBgNVHSMEGDAWgBSx3TJd"\
"6Lc3ctLOXM4m/kd54gEI6TBkBggrBgEFBQcBAQRYMFYwJwYIKwYBBQUHMAGGG2h0"\
"dHA6Ly9vY3NwLnBraS5nb29nL2d0czFkMjArBggrBgEFBQcwAoYfaHR0cDovL3Br"\
"aS5nb29nL2dzcjIvR1RTMUQyLmNydDAbBgNVHREEFDASghBiYXR0ZXJ5aW50ZXIu"\
"bmV0MCEGA1UdIAQaMBgwCAYGZ4EMAQIBMAwGCisGAQQB1nkCBQMwLwYDVR0fBCgw"\
"JjAkoCKgIIYeaHR0cDovL2NybC5wa2kuZ29vZy9HVFMxRDIuY3JsMIIBAwYKKwYB"\
"BAHWeQIEAgSB9ASB8QDvAHYA8JWkWfIA0YJAEC0vk4iOrUv+HUfjmeHQNKawqKqO"\
"snMAAAFzZEh9wwAABAMARzBFAiAUfsKpmntR9k6B5Q6SAmYP6yE8pLsPqTByyRVZ"\
"A+GGOAIhAOAiUM2AXjkKYnjWhHNRxuOeWJLqgwprGDtbqgj3vCPWAHUAB7dcG+V9"\
"aP/xsMYdIxXHuuZXfFeUt2ruvGE6GmnTohwAAAFzZEh9ugAABAMARjBEAiAWZI0A"\
"FJPvpfSgOSByY9FVQ2nIYcrkCS4R4gLQ7TCCNwIgXV5twcQh/yUhnkNppuJBXXU6"\
"OsomiKFuIV6tEmKZCfEwDQYJKoZIhvcNAQELBQADggEBAK1p0Y8vd9sn5pGB+EpO"\
"JW7RVxvL4vNfVFb3+vHfdshvl2gXwNZW22Y9Z1gkz3S/7da23E9mWAe4JmSgwm5W"\
"/dXb0n6P2C0Ipz/K18SCaoMVPQFurYMIShZxpmuRhxR0EsNrAlvVusZqwZ7qd18s"\
"7Bqv4KOIBk39X3r5Tly7wC+FquGOZjH7eugoh06JxdU5zUuxCkSXEwf47v1JP05c"\
"JItHIeGtGHRWZ3Nn3mX/XRx4RESjB+WfxaR4IYTGgOurB2vzXec4eEFZC86ftjx9"\
"uIPNal/5Snl4vC+JIJGXfQvxWcQukTlBPHBwTPJsx5SL9nPLn8akD2+TFhQcOAbm"\
"Q+s="\
"-----END CERTIFICATE-----";
    esp_http_client_config_t config;
    
    memset(&config, 0, sizeof(config));
    config.url = uri;
    config.cert_pem = CAcert;
    config.method = HTTP_METHOD_POST;

    //this method to be called by itero .... wifi.localip pass in or is there a global avail?
    //POST to batteryinter.net/connect w/ header of IoB - registration

    String data; //this object has {"data":"message"} , change to be structure website wants
        //$post= @{'macAddrBat'='192.168.0.32';'publicIP'='255.255.255.24';'data'='JBFburninatingAllTheGromflomites42'}

    const char *placeholder = "someAddr";
    data = "[{'macAddrBat'='";
//    IPAddress ip_address = WiFi.localIP();
//    String address_string = String(ip_address[0]) + String(".") +  String(ip_address[1]) + String(".") +  String(ip_address[2]) + String(".") +  String(ip_address[3]);
    data += (char *)placeholder;
    data += "'";

    data = "{'publicIP'='";
    data += (char *)placeholder;
    data += "'";

    data += "{'data'='";
    data += (char *)message;
    data += "'}]";

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_post_field(client, data.c_str(),(int) data.length());

    String header_key = "IoBkey";
    String header_value = "IoBvalue";
    esp_err_t err_header = esp_http_client_set_header(client, header_key.c_str(), header_value.c_str()); //not handling return: ESP_FAIL/ESP_OK
    if(err_header == ESP_FAIL){
        Serial.println("boi_client.cpp header Error msg: " + err_header);
    }

    esp_err_t err = esp_http_client_perform(client);
    if(err == ESP_FAIL){
        Serial.println("boi_client.cpp esp_http_client_perform error: " + err);
    }

    if (err == ESP_OK) { //does header failure mean client is in a bad state and cannot post?
        ESP_LOGI(TAG, "Status = %d, content_length = %d",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    }
    
    // get back the content here, do we want to pass this back to something calling into here or pass back on some medium
        //something else is listening to?

    esp_http_client_cleanup(client);
}
