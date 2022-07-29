#!/bin/sh

mosquitto_pub -V 5  -h localhost -p 1883 -u testusername -P testpassword -t "ams/han" -m "{msg.payload.data.P: 0;}"
