#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "esp_log.h"


#define	WIFI_SSID	"AT_Wireless-2"
#define WIFI_PASSWORD	"xxxxxxx"


esp_err_t event_handler(void *ctx, system_event_t *event)
{
	if(event->event_id == SYSTEM_EVENT_STA_START)
	{
		esp_wifi_connect();
	}
	else if(event->event_id == SYSTEM_EVENT_STA_GOT_IP)
	{
		printf("Connected to A WiFi AP!\n");
		//print the local IP address
		tcpip_adapter_ip_info_t ip_info;

		ESP_ERROR_CHECK(tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ip_info));
		printf("IP Address: %s\n", ip4addr_ntoa(&ip_info.ip));
		printf("Subnet Mask: %s\n", ip4addr_ntoa(&ip_info.netmask));
		printf("Gateway:     %s\n", ip4addr_ntoa(&ip_info.gw));

		esp_wifi_disconnect();
	}
	else if(event->event_id == SYSTEM_EVENT_STA_DISCONNECTED)
	{
		printf("Disconnected to A WIFI AP %s!\n",WIFI_SSID);
	}

	return ESP_OK;


}

int app_main(void)
{
	//Initiliaze the NVS Flash
	nvs_flash_init();

	//Disable the default wifi logging
	esp_log_level_set("wifi", ESP_LOG_NONE);

	//Disable stdout buffering
	//setvbuf(stdout, NULL, _IONBF, 0);

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
	
	//configure the wifi connection
	wifi_config_t wifiConf = { 
	  .sta = {
		.ssid = WIFI_SSID,
		.password = WIFI_PASSWORD,
	  }
	};

	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA,&wifiConf));

	printf("\nStarting to Connect to Wifi Acess Point %s\n",WIFI_SSID);
	ESP_ERROR_CHECK(esp_wifi_start());

	return 0;
}
