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

For a step by step guide, look at the "Getting Start with Bluetooth SDK" document.

## GATT Attributes
