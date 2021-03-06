/*------------------------------------------------------------------------------------------------
ZochoTen Smart Garden EcoSystem 

The MIT License (MIT)
Copyright © 2020 Juuis Le

Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
and associated documentation files (the “Software”), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, 
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software 
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies 
or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS 
OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
---------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include "esp_system.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "freertos/FreeRTOS.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

#include "zocho_mqtt.h"
#include "zocho_gpio.h"

static const char *TAG = "MQTT";
#define BROKER_URL "mqtt://64.225.97.160:8883"
esp_mqtt_client_handle_t client;

static esp_err_t mqtt_event_handler_execute(esp_mqtt_event_handle_t event)
{
  switch (event->event_id)
  {
  case MQTT_EVENT_CONNECTED:
    ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
    zocho_mqtt_publish("device/solenoid/status", "mqtt_connected", 1);
    zocho_mqtt_subscribe("device/solenoid/control", 1);
    break;
 
  /* This is a very urgly piece of code */
  case MQTT_EVENT_DATA:
    ESP_LOGI(TAG, "MQTT_EVENT_DATA");
    char topic[30];
    char data[10];
    sprintf(topic, "%.*s", event->topic_len, event->topic);
    sprintf(data, "%.*s", event->data_len, event->data);
    if (strcmp(topic, "data/moisture") == 0)
    {
      printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
      printf("DATA=%.*s\r\n", event->data_len, event->data);
    }
    else if (strcmp(topic, "device/solenoid/control") == 0)
    {
      printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
      printf("DATA=%.*s\r\n", event->data_len, event->data);
      if (strcmp(data, "ON") == 0)
      {
        zocho_gpio_solenoid1_on();
      }
      else if (strcmp(data, "OFF") == 0)
      {
        zocho_gpio_solenoid1_off();
      }
    }
    break;

  case MQTT_EVENT_PUBLISHED:
    ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
    break;
  case MQTT_EVENT_SUBSCRIBED:
    ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
    zocho_mqtt_publish("/device/status", event->topic, 1);
    break;
  case MQTT_EVENT_UNSUBSCRIBED:
    ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED");
    break;
  case MQTT_EVENT_DISCONNECTED:
    ESP_LOGI(TAG, "MQTT_DISCONNECTED");
    break;
  case MQTT_EVENT_ERROR:
    ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
    break;
  default:
    break;
  }
  return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
  mqtt_event_handler_execute(event_data);
}

void zocho_mqtt_init()
{
  const esp_mqtt_client_config_t mqtt_cfg = {
    .uri = BROKER_URL,
  };

  client = esp_mqtt_client_init(&mqtt_cfg);
  esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
  esp_mqtt_client_start(client);
}

int zocho_mqtt_publish(const char *topic, const char *payload, const int qos)
{
  return esp_mqtt_client_publish(client, topic, payload, 0, qos, 0);
}

int zocho_mqtt_subscribe(const char *topic, const int qos)
{
  return esp_mqtt_client_subscribe(client, topic, qos);
}
