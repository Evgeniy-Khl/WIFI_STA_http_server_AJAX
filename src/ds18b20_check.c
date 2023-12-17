#include "ds18b20_check.h"
//-------------------------------------------------------
extern int num_devices;
extern float readings[];
static const char *TAG = "ds18b20_check";
//-------------------------------------------------------
void ds18b20_get_readings(void *ignore)
{

  ESP_LOGI(TAG, "OWB Start");
  // Create a 1-Wire bus, using the RMT timeslot driver
  OneWireBus * owb;
  owb_rmt_driver_info rmt_driver_info;
  owb = owb_rmt_initialize(&rmt_driver_info, CONFIG_ONE_WIRE_GPIO, RMT_CHANNEL_1, RMT_CHANNEL_0);
  owb_use_crc(owb, true);  // enable CRC check for ROM code

  // Find all connected devices
  ESP_LOGI(TAG, "Find devices");
  OneWireBus_ROMCode device_rom_codes[CONFIG_MAX_DEVICES] = {0};
  
  OneWireBus_SearchState search_state = {0};
  bool found = false;
  owb_search_first(owb, &search_state, &found);
  while (found)
  {
    char rom_code_s[17];
    owb_string_from_rom_code(search_state.rom_code, rom_code_s, sizeof(rom_code_s));
    printf("  %d : %s\n", num_devices, rom_code_s);
    device_rom_codes[num_devices] = search_state.rom_code;
    ++num_devices;
    owb_search_next(owb, &search_state, &found);
  }
  printf("Found %d device%s\n", num_devices, num_devices == 1 ? "" : "s");
  if (num_devices == 1)
  {
    OneWireBus_ROMCode rom_code;
    owb_status status = owb_read_rom(owb, &rom_code);
    if (status == OWB_STATUS_OK)
    {
      char rom_code_s[OWB_ROM_CODE_STRING_LENGTH];
      owb_string_from_rom_code(rom_code, rom_code_s, sizeof(rom_code_s));
      printf("Single device %s present\n", rom_code_s);
    }
    else
    {
      printf("An error occurred reading ROM code: %d", status);
    }
  }
  
  // Create DS18B20 devices on the 1-Wire bus
  DS18B20_Info * devices[CONFIG_MAX_DEVICES] = {0};
  for (int i = 0; i < num_devices; ++i)
  {
    DS18B20_Info * ds18b20_info = ds18b20_malloc();  // heap allocation
    devices[i] = ds18b20_info;
    if (num_devices == 1)
    {
        printf("Single device optimisations enabled\n");
        ds18b20_init_solo(ds18b20_info, owb);          // only one device on bus
    }
    else
    {
        ds18b20_init(ds18b20_info, owb, device_rom_codes[i]); // associate with bus and device
    }
    ds18b20_use_crc(ds18b20_info, true);           // enable CRC check on all reads
    ds18b20_set_resolution(ds18b20_info, DS18B20_RESOLUTION);
  }
    ds18b20_convert_all(owb);
    vTaskDelay(1000 / portTICK_PERIOD_MS);

  if (num_devices > 0)
  {
    int errors_count[CONFIG_MAX_DEVICES] = {0};
    while (1)
    {   
        TickType_t last_wake_time = xTaskGetTickCount();    
        
        // In this application all devices use the same resolution,
        // so use the first device to determine the delay
        // ds18b20_wait_for_conversion(devices[0]);
        // Read the results immediately after conversion otherwise it may fail
        // (using printf before reading may take too long)
        
        DS18B20_ERROR errors[CONFIG_MAX_DEVICES] = { 0 };
        for (int i = 0; i < num_devices; ++i)
        {
            errors[i] = ds18b20_read_temp(devices[i], &readings[i]);
        }
        for (int i = 0; i < num_devices; ++i)
        {
            if (errors[i] != DS18B20_OK) ++errors_count[i];
            printf("  %d: %.1f    %d errors\n", i, readings[i], errors_count[i]);
        }
        ds18b20_convert_all(owb);
        vTaskDelayUntil(&last_wake_time, 1000 / portTICK_PERIOD_MS);
    }
  }
  else
  {
    printf("\nNo DS18B20 devices detected!\n");
  }
  for (int i = 0; i < num_devices; ++i)
  {
      ds18b20_free(&devices[i]);
  }
  owb_uninitialize(owb);
  vTaskDelete(NULL);
  printf("Restarting now.\n");
  fflush(stdout);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  esp_restart();
}