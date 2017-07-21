#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"

static char* get_authmode(wifi_auth_mode_t authmode)
{
	char *mode_name[]={"OPEN", "WEP", "WPA PSK", "WPA2 PSK", "WPA WPA2 PSK", "MAX"};
	return mode_name[authmode];
}


esp_err_t event_handler(void *ctx, system_event_t *event)
{
	if(event->event_id == SYSTEM_EVENT_SCAN_DONE)
	{
		printf("WiFi Scan Completed!\n");
		printf("Number of access points found: %d\n",event->event_info.scan_done.number);
		uint16_t apCount = event->event_info.scan_done.number;
		if(apCount == 0)
		{
			return 0;
		}
		wifi_ap_record_t *list = (wifi_ap_record_t *) malloc(sizeof(wifi_ap_record_t) *apCount);
		ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&apCount, list));

		printf("\n");
	        printf("               SSID              | Channel | RSSI |   Auth Mode \n");
       		printf("----------------------------------------------------------------\n");
        	for(int i = 0; i < apCount; i++)
		{
			printf("%32s | %7d | %4d | %12s\n",(char *)list[i].ssid, list[i].primary, list[i].rssi, get_authmode(list[i].authmode));
		}

		printf("----------------------------------------------------------------\n");


		free(list);
	}

	return ESP_OK;


}

int app_main(void)
{
	//Initiliaze the NVS Flash
	nvs_flash_init();

	//Initialize the TCP Stack
	tcpip_adapter_init();

	//Initialize the system event handler
	ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

	//Config the wifi
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	//Config RAM as the wifi parameters storage
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

	//Wifi as Station Mode
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	
	//Start wifi
	ESP_ERROR_CHECK(esp_wifi_start());

	//let us test a wifi scan ...
	wifi_scan_config_t scanConf = { 
		.ssid = NULL,
		.bssid = NULL,
		.channel = 0,
		.show_hidden = 1
	};

	printf("\nStarting to Scan Wifi Acess Point\n");
	ESP_ERROR_CHECK(esp_wifi_scan_start(&scanConf, 0));

	return 0;
}
