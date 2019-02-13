# BLE-Guide

This guide holds what I learn about BLE. It uses Silicon Lab's BGM13P Module.

## Quick Guide
The server-client is the basic name conventions for any BLE device. The server measures and holds the data which a client will read. A peripheral (BGM13P) device is usually the server and a phone is the client for which to monitor the data.

A Generic Attribute Profile (GATT) database must be created before run-time. This cannot be changed once compiled (excluding the data fields). You can think of this as a table where each GATT attribute corresponds to a data field.

With the Bluetooth SDK that Silicon Labs provides, you can easily create the database in the .isc file. It's a simple drag and drop from the left to right column. To have custom ones, look in later section to learn more. Choose what kind of GATT fields you want the client to see.

Once you finish the GATT database, you can use the following function in native_gecko.h to update the data field to a corresponding GATT attribute:

	gecko_cmd_gatt_server_write_attribute_value(uint16 attribute, uint16 offset, uint8 value_len, const uint8* value_data);

*** Note you will need to call the send notification function in native_gecko.h to notify the client that the device updated its data field.

Download Silicon Lab's Blue Gecko App to connect the board to your phone and monitor the data.

For a step by step guide, look at the "Getting Started with Bluetooth SDK" document.

## GATT Attributes


## BLE-soc-basic
This the first project where I test BLE. I followed the "Getting Started..." guide to create one. The .isc file is where you customize and create the GATT database of your device. I created the "MyVoltageService" that holds a 2 byte data field where the voltage of the board can be read. The ID of the characteristic settings needs to be checked and named in order to create a macro definition. I change the ID to be board_voltage which created a gattdb_board_voltage definition. I called the following function in app.c:

	gecko_cmd_gatt_server_write_attribute_value(gattdb_board_voltage, 0, 2, ADCdata);

The offset = 0 and the length of the data = 2 which is 2 bytes. The ADCdata variable holds the voltage of the board. This functions writes the data to the associated GATT attribute for a phone to read.

### Blocking vs non-Blocking
In app.c, the basis of the while loop is a gigantic switch statement that handles different events. The evt variable (struct gecko_cmd_packet* evt) holds the event type to switch between cases. In the default example code, 

	evt = gecko_wait_event();

sets the evt variable. This is a blocking statement. To make this non-blocking, change the line to the following:

	evt = gecko_peek_event();

This will allow you to do something while waiting for an event.
