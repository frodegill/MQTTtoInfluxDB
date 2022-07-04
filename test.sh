#!/bin/sh

mosquitto_pub -h localhost -p 1883 -u username -P password -t "ams/han" -m "{msg.payload.data.P: 0;}"
