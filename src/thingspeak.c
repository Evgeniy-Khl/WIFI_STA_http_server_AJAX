#include "thingspeak.h"
//-------------------------------------------------------
static const char *TAG = "Thing Speak";
extern float readings[];
char api_key[] = "K7ECSMFAOIBODQ98";
//--https://api.thingspeak.com/update?api_key=K7ECSMFAOIBODQ98&field1=20.3&field2=18.7      [82]
void send_data_to_thingspeak(void *pvParameters)
{

	char thingspeak_url[] = "https://api.thingspeak.com";       //  https://eosmrysfdsc0z53.m.pipedream.net;   https://api.thingspeak.com
	char data[] = "/update?api_key=%s&field1=%.1f&field2=%.1f";
	char post_data[100];
	esp_err_t err;

	esp_http_client_config_t config = {
		.url = thingspeak_url,
		.method = HTTP_METHOD_GET,
	};
	esp_http_client_handle_t client = esp_http_client_init(&config);
	esp_http_client_set_header(client, "Content-Type", "application/x-www-form-urlencoded");
	while (1)
	{
		vTaskDelay(30000 / portTICK_PERIOD_MS);
		strcpy(post_data,"");
		snprintf(post_data, sizeof(post_data), data, api_key, readings[0], readings[1]);
		ESP_LOGI(TAG, "post = %s", post_data);
        esp_http_client_set_url(client, post_data);
		// esp_http_client_set_post_field(client, post_data, strlen(post_data));

		err = esp_http_client_perform(client);

		if (err == ESP_OK)
		{
			int status_code = esp_http_client_get_status_code(client);
            int64_t cont_lenght = esp_http_client_get_content_length(client);
			if (status_code == 200)
			{
                ESP_LOGI(TAG, "Message sent Successfully  CONT LENGHT:%lld",cont_lenght);
			}
			else
			{
				ESP_LOGI(TAG, "Message sent Failed   STATUS:%d",status_code);				
				goto exit;
			}
		}
		else
		{
			ESP_LOGI(TAG, "Message sent Failed   ERR:%d",err);
			goto exit;
		}
	}
exit:
	esp_http_client_cleanup(client);
	vTaskDelete(NULL);
}
