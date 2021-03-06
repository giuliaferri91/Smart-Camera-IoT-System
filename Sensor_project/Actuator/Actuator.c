#include "contiki.h"
#include "contiki-net.h"
#include "rest-engine.h"
#include <stdio.h>

static int camera_value = 0;
static int room_number = 0;

void get_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
void post_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
void get_camera_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
void post_camera_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);


RESOURCE(setting, "title=\"setting\";type=\"CAMERA\";rt=\"room_number\"", get_handler, post_handler, NULL, NULL);
RESOURCE(camera,"title=\"camera\";rt=\"camera_value\";if=\"0\"",get_camera_handler,post_camera_handler,NULL,NULL);

void get_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
	/* Populat the buffer with the response payload*/
	char message[30];
	int length = 30;
	
	sprintf(message, "%u",room_number);
	length = strlen(message);
	memcpy(buffer, message, length);

	REST.set_header_content_type(response, REST.type.TEXT_PLAIN); 
	REST.set_header_etag(response, (uint8_t *) &length, 1);
	REST.set_response_payload(response, buffer, length);
}

void post_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
	int len = 10, new_value;
	const char *val = NULL;

	len = REST.get_post_variable(request,"room",&val);
	
	if( len > 0 ){
    	new_value = atoi(val);	
    	room_number = new_value;
    	REST.set_response_status(response, REST.status.CREATED);
    } 
    else 
    	REST.set_response_status(response, REST.status.BAD_REQUEST);
}


void get_camera_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
	/* Populat the buffer with the response payload*/
	char message[10];
	int length = 10;
	
	sprintf(message, "%u", camera_value);
	length = strlen(message);
	memcpy(buffer, message, length);

	REST.set_header_content_type(response, REST.type.TEXT_PLAIN); 
	REST.set_header_etag(response, (uint8_t *) &length, 1);
	REST.set_response_payload(response, buffer, length);
}

// activate/deactivate the camera
void post_camera_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
	int len = 30, new_value;
	const char *val = NULL;

	len = REST.get_post_variable(request,"camera_set",&val);
	
	if( len > 0 ){
    	new_value = atoi(val);	
    	if(new_value ==1 || new_value == 0)
    	{
    		camera_value = new_value;
    		REST.set_response_status(response, REST.status.CREATED);
    	}
    	else
    		REST.set_response_status(response, REST.status.NOT_ACCEPTABLE);
    } 
    else 
    	REST.set_response_status(response, REST.status.BAD_REQUEST);
  	
}


PROCESS(server, "CoAP Server");
AUTOSTART_PROCESSES(&server);
PROCESS_THREAD(server, ev, data)
{
	PROCESS_BEGIN();
	rest_init_engine();
	rest_activate_resource(&setting, "setting");
	rest_activate_resource(&camera, "camera");
	while(1) {
   		 PROCESS_WAIT_EVENT();
	}
	PROCESS_END();
}