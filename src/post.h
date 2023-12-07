#ifndef POST_H_
#define POST_H_
//-------------------------------------------------------------
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include <sys/param.h>
#include <esp_http_server.h>
#include "esp_vfs.h"
//-------------------------------------------------------------
void check_post_query(httpd_req_t *req, size_t buf_len);
//-------------------------------------------------------------
#endif /* POST_H_ */
