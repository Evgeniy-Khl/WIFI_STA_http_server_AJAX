#include "http.h"

#define GPIO_OUT_REG          (DR_REG_GPIO_BASE + 0x0004)
//-------------------------------------------------------------
static const char *TAG = "post";
float temperature = 25.0;
float humidity = 65.0;
//-------------------------------------------------------------
#define NUM_RELAYS  5   // Set number of relays
int relayGPIOs[NUM_RELAYS] = {4, 5, 18, 19, 13};  // Assign each GPIO to a relay
//-------------------------------------------------------------
#define IS_FILE_EXT(filename, ext) \
    (strcasecmp(&filename[strlen(filename) - sizeof(ext) + 1], ext) == 0)
//-------------------------------------------------------------
float getTemperature(){
  temperature = temperature + 0.1;
  return temperature;
}
  
float getHumidity() {
  humidity = humidity - 0.1;
  return humidity;
}
//-------------------------------------------------------------
void check_post_query(httpd_req_t *req, size_t buf_len)
{
  char*  buf;

  buf = malloc(buf_len);  // динамическим распределением памяти

      if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK)
      {
        ESP_LOGI(TAG, "Found POST_handler query => %s", buf);
        char param[32];

        if (httpd_query_key_value(buf, "red", param, sizeof(param)) == ESP_OK)
        {
           ESP_LOGI(TAG, "Found POST_handler query parameter => red:%s", param);
           if(!strcmp(param,"1")) gpio_set_level(CONFIG_RED_GPIO, 0);
           else if(!strcmp(param,"0")) gpio_set_level(CONFIG_RED_GPIO, 1);
        }

        else if (httpd_query_key_value(buf, "green", param, sizeof(param)) == ESP_OK)
        {
           ESP_LOGI(TAG, "Found POST_handler query parameter => green:%s", param);
           if(!strcmp(param,"1")) gpio_set_level(CONFIG_GREEN_GPIO, 0);
           else if(!strcmp(param,"0")) gpio_set_level(CONFIG_GREEN_GPIO, 1);
        }

        else if (httpd_query_key_value(buf, "blue", param, sizeof(param)) == ESP_OK)
        {
           ESP_LOGI(TAG, "Found POST_handler query parameter => blue:%s", param);
           if(!strcmp(param,"1")) gpio_set_level(CONFIG_BLUE_GPIO, 0);
           else if(!strcmp(param,"0")) gpio_set_level(CONFIG_BLUE_GPIO, 1);
        }
        
        else if (httpd_query_key_value(buf, "status", param, sizeof(param)) == ESP_OK)
        {
           unsigned int reg_out = *(unsigned int*) GPIO_OUT_REG;
           char *end; 
           int num;
           char *resp_str = NULL;
           resp_str = (char*) req->user_ctx;
           resp_str = malloc(4);
           
           num = strtod(param, &end);
           num = relayGPIOs[num];
           ESP_LOGI(TAG, "Found POST_handler query parameter => GPIO:%u  reg_out=%u", num, reg_out);
           if((reg_out >> num) & 0x1) strcpy(resp_str,"OFF");
           else strcpy(resp_str,"ON");// добавляет strSource в strDestination и завершает результирующую строку символом NULL.(рекомендуется использовать strncat)
           ESP_LOGI(TAG, "Response line: %s", resp_str);
           httpd_resp_send(req, resp_str, strlen(resp_str));
           free(resp_str);
        }
        else if (httpd_query_key_value(buf, "update", param, sizeof(param)) == ESP_OK)
        {
            char state[4];
            char *resp_str = NULL;
            resp_str = (char*) req->user_ctx;
            resp_str = malloc(4);
            httpd_query_key_value(buf, "state", state, sizeof(param));
           // param = "rel1"; state = "1";
            ESP_LOGI(TAG, "Found POST_handler query parameter => update:%s  state:%s", param, state);
            if(!strncmp(param,"rel1",4)){
               if (!strcmp(state,"1")) {gpio_set_level(CONFIG_WHITE_GPIO, 0); strcpy(resp_str,"ON");}
               else if(!strcmp(state,"0")) {gpio_set_level(CONFIG_WHITE_GPIO, 1); strcpy(resp_str,"OFF");}
            }
            else if(!strncmp(param,"rel2",4)){
               if (!strcmp(state,"1")) {gpio_set_level(CONFIG_RED_GPIO, 0); strcpy(resp_str,"ON");}
               else if(!strcmp(state,"0")) {gpio_set_level(CONFIG_RED_GPIO, 1); strcpy(resp_str,"OFF");}
            }
            else if(!strncmp(param,"rel3",4)){
               if (!strcmp(state,"1")) {gpio_set_level(CONFIG_GREEN_GPIO, 0); strcpy(resp_str,"ON");}
               else if(!strcmp(state,"0")) {gpio_set_level(CONFIG_GREEN_GPIO, 1); strcpy(resp_str,"OFF");}
            }
            else if(!strncmp(param,"rel4",4)){
               if (!strcmp(state,"1")) {gpio_set_level(CONFIG_BLUE_GPIO, 0); strcpy(resp_str,"ON");}
               else if(!strcmp(state,"0")) {gpio_set_level(CONFIG_BLUE_GPIO, 1); strcpy(resp_str,"OFF");}
            } 
            httpd_resp_send(req, resp_str, strlen(resp_str));
            free(resp_str);  
        }
        else if (httpd_query_key_value(buf, "temperature", param, sizeof(param)) == ESP_OK)
        {
           ESP_LOGI(TAG, "Found POST_handler query parameter => temperature:%s", param);
           sprintf(buf, "%.1f", getTemperature());
           printf("resp: %s\n", buf);
           httpd_resp_send(req, buf, strlen(buf));
        }
        else if (httpd_query_key_value(buf, "humidity", param, sizeof(param)) == ESP_OK)
        {
           ESP_LOGI(TAG, "Found POST_handler query parameter => humidity:%s", param);
           sprintf(buf, "%.1f", getHumidity());
           printf("resp: %s\n", buf);
           httpd_resp_send(req, buf, strlen(buf));
        }
      //   free(resp_str);
      }
      free(buf);
}