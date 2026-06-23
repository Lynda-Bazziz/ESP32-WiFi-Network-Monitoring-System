#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_timer.h"
#include "freertos/timers.h"

#include "esp_wifi.h"
#include "nvs_flash.h"
#include "lwip/sys.h" //tct_stack
#include "lwip/err.h" //tcp_stack
#include "freertos/event_groups.h"



//tcpip_adapter_init() pour initialiser le reseau
//espevent_loop_init() pour initialiser le event loop


//nvs_flash_init() //initialisation des adresse mac


EventGroupHandle_t survaillance;
                                 
    esp_err_t event_handler( void* ctx, system_event_t *event ){
                 switch (event->event_id)
                 {
                 case (SYSTEM_EVENT_STA_START):
                    printf("\n entrain de se connecter");
                    break;
                 
                case (SYSTEM_EVENT_STA_CONNECTED):
                    printf("\n connected");
                    break;

                case (SYSTEM_EVENT_STA_DISCONNECTED):
                    printf("\n connextion echouer");
                    esp_wifi_connect();
                    break;

                 case (SYSTEM_EVENT_STA_GOT_IP):
                    printf("\n entrain de se connecter");
                     xEventGroupSetBits(survaillance,1);
                    break;
                 
                 default:
                 printf("\n switch case a echouer");
                    break;
                 }
    }

void init_config (){

                            //////////1)INITIALISSATTIOONNNNNNNNN//////////
tcpip_adapter_init();
espevent_loop_init(event_handler,NULL);
    
wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
esp_wifi_init(&cfg);

                           /////////////2)configuration)//////////////////////
esp_wifi_set_mode(WIFI_MODE_STA);
wifi_config_t configuration = {
   .sta={
     .ssid= "Test",
     .password = "Essai2020",
   },

};
esp_wifi_start();
SYSTEM_EVENT_STA_START;
esp_wifi_set_config(ESP_IF_WIFI_STA , &configuration);
}

void tache_utili(void* parameters){
xEventGroupWaitBits(survaillance,1,pdTRUE,pdTRUE,portMAX_DELAY);
char* ip4addr_ntoa(const ip4_addr_t * addr);

while (1)
{
       printf("Configuration réseau :\n");
        tcpip_adapter_ip_info_t ipInfo;
        tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ipInfo);
        printf("- Adresse IP : %s\n", ip4addr_ntoa(&ipInfo.ip));
        printf("- Masque de sous-réseau : %s\n", ip4addr_ntoa(&ipInfo.netmask));
        printf("- Passerelle : %s\n", ip4addr_ntoa(&ipInfo.gw));

        vTaskDelay(pdMS_TO_TICKS(2000));
}

}

void app_main(void)
{
survaillance = xEventGroupCreate();
nvs_flash_init();
init_config();
xTaskCreate(survaillance,"utili",1024,NULL,4,NULL);
}




#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "tcpip_adapter.h"

#define BLUE_LED_GPIO GPIO_NUM_2
#define RED_LED_GPIO GPIO_NUM_4

EventGroupHandle_t surveillance;

esp_err_t event_handler(void* ctx, system_event_t *event) {
    switch (event->event_id) {
        case SYSTEM_EVENT_STA_START:
            printf("\n En train de se connecter");
            break;

         case :SYSTEM_EVENT_STA_CONNECTED : 
            printf("\n Connecté à l'adresse IP");
            gpio_set_level(BLUE_LED_GPIO, 1);  // Allumer la LED bleue
            gpio_set_level(RED_LED_GPIO, 0);   // Éteindre la LED rouge
            xEventGroupSetBits(surveillance, 1);
            break;          

        case SYSTEM_EVENT_STA_DISCONNECTED:
            printf("\n Connexion échouée");
            gpio_set_level(BLUE_LED_GPIO, 0);  // Éteindre la LED bleue
            gpio_set_level(RED_LED_GPIO, 1);   // Allumer la LED rouge
            esp_wifi_connect();
            break;

         case SYSTEM_EVENT_STA_GOT_IP:
            printf("\n Connecté");
            break;  

        default:
            printf("\n Switch case a échoué");
            break;
    }

    return ESP_OK;
}

void init_leds() {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL<<BLUE_LED_GPIO) | (1ULL<<RED_LED_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
    };
    gpio_config(&io_conf);
}

void init_config() {
    tcpip_adapter_init();
    esp_event_loop_init(event_handler, NULL);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_wifi_set_mode(WIFI_MODE_STA);

    wifi_config_t configuration = {
        .sta={
            .ssid= "Test",
            .password = "Essai2020",
        },
    };

    esp_wifi_set_config(ESP_IF_WIFI_STA, &configuration);
    esp_wifi_start();
}

void tache_utili(void* parameters) {
    xEventGroupWaitBits(surveillance, 1, pdFALSE, pdTRUE, portMAX_DELAY);

    while (1) {
        printf("Configuration réseau :\n");
        tcpip_adapter_ip_info_t ipInfo;
        tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ipInfo);
        printf("- Adresse IP : %s\n", ip4addr_ntoa(&ipInfo.ip));
        printf("- Masque de sous-réseau : %s\n", ip4addr_ntoa(&ipInfo.netmask));
        printf("- Passerelle : %s\n", ip4addr_ntoa(&ipInfo.gw));

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void app_main(void) {
    surveillance = xEventGroupCreate();
    nvs_flash_init();
    init_config();
    init_leds();

    xTaskCreate(tache_utili, "utili", 1024, NULL, 4, NULL);
}
