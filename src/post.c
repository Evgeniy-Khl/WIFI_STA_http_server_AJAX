#include "http.h"
#define CONFIG_RED_GPIO 21
#define CONFIG_GREEN_GPIO 22
#define CONFIG_BLUE_GPIO 23
#define GPIO_OUT_REG          (DR_REG_GPIO_BASE + 0x0004)
//-------------------------------------------------------------
static const char *TAG = "post";
float temperature = 25.0;
float humidity = 65.0;
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
/* void send_table(httpd_req_t *req)
{
  char *resp_str = NULL;
  resp_str = (char*) req->user_ctx;
  resp_str = malloc(200);

  unsigned int reg_out = *(unsigned int*) GPIO_OUT_REG;

  strcpy(resp_str,"<table>\
  <tr><th>RED</th><th>GREEN</th><th>BLUE</th></tr>\
  <tr><td>");// копирует строку символов из источника <base_path> в пункт назначения <dest>
  if((reg_out >> CONFIG_RED_GPIO) & 0x1) strcat(resp_str,"OFF");
  else strcat(resp_str,"ON");// добавляет strSource в strDestination и завершает результирующую строку символом NULL.(рекомендуется использовать strncat)
  strcat(resp_str,"</td><td>");
  if((reg_out >> CONFIG_GREEN_GPIO) & 0x1) strcat(resp_str,"OFF");
  else strcat(resp_str,"ON");
  strcat(resp_str,"</td><td>");
  if((reg_out >> CONFIG_BLUE_GPIO) & 0x1) strcat(resp_str,"OFF");
  else strcat(resp_str,"ON");
  strcat(resp_str,"</td></tr></table>");

  httpd_resp_send(req, resp_str, strlen(resp_str));
} */
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
         //   send_table(req);
        }

        else if (httpd_query_key_value(buf, "green", param, sizeof(param)) == ESP_OK)
        {
           ESP_LOGI(TAG, "Found POST_handler query parameter => green:%s", param);
           if(!strcmp(param,"1")) gpio_set_level(CONFIG_GREEN_GPIO, 0);
           else if(!strcmp(param,"0")) gpio_set_level(CONFIG_GREEN_GPIO, 1);
         //   send_table(req);
        }

        else if (httpd_query_key_value(buf, "blue", param, sizeof(param)) == ESP_OK)
        {
           ESP_LOGI(TAG, "Found POST_handler query parameter => blue:%s", param);
           if(!strcmp(param,"1")) gpio_set_level(CONFIG_BLUE_GPIO, 0);
           else if(!strcmp(param,"0")) gpio_set_level(CONFIG_BLUE_GPIO, 1);
         //   send_table(req);
        }
        
        else if (httpd_query_key_value(buf, "status", param, sizeof(param)) == ESP_OK)
        {
           unsigned int reg_out = *(unsigned int*) GPIO_OUT_REG;
           int base = 2;
           char *end; 
           long int num;
           char *resp_str = NULL;
           resp_str = (char*) req->user_ctx;
           resp_str = malloc(4);

           ESP_LOGI(TAG, "Found POST_handler query parameter => status:%s", param);
           num = strtol(param, &end, base);
           if((reg_out >> num) & 0x1) strcpy(resp_str,"OFF");
           else strcpy(resp_str,"ON");// добавляет strSource в strDestination и завершает результирующую строку символом NULL.(рекомендуется использовать strncat)
           ESP_LOGI(TAG, "Response line: %s", resp_str);
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
      //   else send_table(req);
       
      //   free(resp_str);
      }
      free(buf);
}