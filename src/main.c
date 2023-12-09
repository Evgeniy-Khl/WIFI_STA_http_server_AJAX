#include "main.h"

//-------------------------------------------------------------
static const char *TAG = "main";
//-------------------------------------------------------------
void app_main(void)
{
  gpio_reset_pin(CONFIG_LED_GPIO);
  gpio_set_direction(CONFIG_LED_GPIO, GPIO_MODE_OUTPUT);
  gpio_set_level(CONFIG_LED_GPIO, 0);
  gpio_reset_pin(CONFIG_RED_GPIO);
  gpio_set_direction(CONFIG_RED_GPIO, GPIO_MODE_OUTPUT);
  gpio_set_level(CONFIG_RED_GPIO, 1);
  gpio_reset_pin(CONFIG_GREEN_GPIO);
  gpio_set_direction(CONFIG_GREEN_GPIO, GPIO_MODE_OUTPUT);
  gpio_set_level(CONFIG_GREEN_GPIO, 1);
  gpio_reset_pin(CONFIG_BLUE_GPIO);
  gpio_set_direction(CONFIG_BLUE_GPIO, GPIO_MODE_OUTPUT);
  gpio_set_level(CONFIG_BLUE_GPIO, 1);
  //=============================== Initialize NVS ============================================
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) 
  {
    ret = nvs_flash_erase();
    ESP_LOGI(TAG, "nvs_flash_erase: 0x%04x", ret);
    ret = nvs_flash_init();
    ESP_LOGI(TAG, "nvs_flash_init: 0x%04x", ret);
  }
  ESP_LOGI(TAG, "nvs_flash_init: 0x%04x", ret);

  //listing all the key-value pairs of any type under specified partition and namespace
  nvs_iterator_t it = NULL;
  ret = nvs_entry_find("nvs", NULL, NVS_TYPE_ANY, &it);
  while (it != NULL) 
  {
          nvs_entry_info_t info;
          nvs_entry_info(it, &info);
          ret = nvs_entry_next(&it);
          printf("NVS -> key '%s', type '%02x' \n", info.key, info.type);
  };
  nvs_release_iterator(it);

nvs_handle_t my_handle;
  // Open
  printf("\n");
  printf("Opening Non-Volatile Storage (NVS) handle... ");
  ret = nvs_open("nvs", NVS_READWRITE, &my_handle);
  if (ret != ESP_OK) printf("Error (%s) opening NVS handle!\n", esp_err_to_name(ret));
  else
  {
    printf("Done\n");
    // Write
    int8_t val01 = -108;
    uint8_t val02 = 198;
    int16_t val03 = -10135;
    uint16_t  val04 = 61145;
    int32_t val05 = -101459;
    uint32_t val06 = 3352582294;
    int64_t val07 = -4047512760478312334;
    uint64_t val08 = 7214561357951254753;
    // Read
    int8_t res01 = 0;
    uint8_t res02 = 0;
    int16_t res03 = 0;
    uint16_t res04 = 0;
    int32_t res05 = 0;
    uint32_t res06 = 0;
    int64_t res07 = 0;
    uint64_t res08 = 0;
    printf("Updating data in NVS ... ");
    ret = nvs_set_i8(my_handle, "dt01", val01);
    printf((ret != ESP_OK) ? "Failed!\n" : "Done\n");
    ret = nvs_set_u8(my_handle, "dt02", val02);
    printf((ret != ESP_OK) ? "Failed!\n" : "Done\n");
    ret = nvs_set_i16(my_handle, "dt03", val03);
    printf((ret != ESP_OK) ? "Failed!\n" : "Done\n");
    ret = nvs_set_u16(my_handle, "dt04", val04);
    printf((ret != ESP_OK) ? "Failed!\n" : "Done\n");
    ret = nvs_set_i32(my_handle, "dt05", val05);
    printf((ret != ESP_OK) ? "Failed!\n" : "Done\n");
    ret = nvs_set_u32(my_handle, "dt06", val06);
    printf((ret != ESP_OK) ? "Failed!\n" : "Done\n");
    ret = nvs_set_i64(my_handle, "dt07", val07);
    printf((ret != ESP_OK) ? "Failed!\n" : "Done\n");
    ret = nvs_set_u64(my_handle, "dt08", val08);
    printf((ret != ESP_OK) ? "Failed!\n" : "Done\n");
    
    ret = nvs_set_str(my_handle, "grvSSID", "grvLogin");
    printf((ret != ESP_OK) ? "Failed!\n" : "Done\n");
    ret = nvs_set_str(my_handle, "grvPASS", "123grv7890");
    printf((ret != ESP_OK) ? "Failed!\n" : "Done\n");

    printf("Committing updates in NVS ... ");
    ret = nvs_commit(my_handle);
    printf((ret != ESP_OK) ? "Failed!\n" : "Done\n");

    nvs_get_i8(my_handle, "dt01", &res01);
    printf("dt01: %d\n", res01);
    nvs_get_u8(my_handle, "dt02", &res02);
    printf("dt02: %d\n", res02);
    nvs_get_i16(my_handle, "dt03", &res03);
    printf("dt03: %d\n", res03);
    nvs_get_u16(my_handle, "dt04", &res04);
    printf("dt04: %d\n", res04);
    nvs_get_i32(my_handle, "dt05", &res05);
    printf("dt05: %li\n", res05);
    nvs_get_u32(my_handle, "dt06", &res06);
    printf("dt06: %lu\n", res06);
    nvs_get_i64(my_handle, "dt07", &res07);
    printf("dt07: %lld\n", res07);
    nvs_get_u64(my_handle, "dt08", &res08);
    printf("dt08: %llu\n", res08);

    printf("Reading MySSID from NVS ... ");
    size_t nvs_required_size;
    ret = nvs_get_str(my_handle, "grvSSID", NULL, &nvs_required_size);
    switch (ret)
    {
        case ESP_OK:
            printf("Done\n");
            char *nvs_ret_data;
            nvs_ret_data = malloc(nvs_required_size);
            ret = nvs_get_str(my_handle, "grvSSID", nvs_ret_data, &nvs_required_size );
            printf("grvSSID: %s\n", nvs_ret_data);
            free(nvs_ret_data);
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            printf("The value is not initialized yet!\n");
            break;
        default :
            printf("Error (%s) reading!\n", esp_err_to_name(ret));
    }
 
    printf("Reading MyPASS from NVS ... ");
    ret = nvs_get_str(my_handle, "grvPASS", NULL, &nvs_required_size);
    switch (ret)
    {
        case ESP_OK:
            printf("Done\n");
            char *nvs_ret_data;
            nvs_ret_data = malloc(nvs_required_size);
            ret = nvs_get_str(my_handle, "grvPASS", nvs_ret_data, &nvs_required_size );
            printf("grvPASS: %s\n", nvs_ret_data);
            free(nvs_ret_data);
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            printf("The value is not initialized yet!\n");
            break;
        default :
            printf("Error (%s) reading!\n", esp_err_to_name(ret));
    }
    // Close
    nvs_close(my_handle);
  }

  //====================================================================================
  ESP_LOGI(TAG, "Initializing SPIFFS");

  esp_vfs_spiffs_conf_t conf = {
    .base_path = "/spiffs",
    .partition_label = NULL,
    .max_files = 5,
    .format_if_mount_failed = true
  };

  ret = esp_vfs_spiffs_register(&conf);

  if (ret != ESP_OK) {
      if (ret == ESP_FAIL) {
          ESP_LOGE(TAG, "Failed to mount or format filesystem");
      } else if (ret == ESP_ERR_NOT_FOUND) {
          ESP_LOGE(TAG, "Failed to find SPIFFS partition");
      } else {
          ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
      }
      return;
  }

  size_t total = 0, used = 0;
  ret = esp_spiffs_info(conf.partition_label, &total, &used);
  if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
  } else {
      ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
  }

  ret = esp_netif_init();
  ESP_LOGI(TAG, "esp_netif_init: %d", ret);
  ret = esp_event_loop_create_default();
  ESP_LOGI(TAG, "esp_event_loop_create_default: %d", ret);
  ret = wifi_init_sta();
  ESP_LOGI(TAG, "wifi_init_sta: %d", ret);
}
