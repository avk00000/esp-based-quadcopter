#pragma once
#include <Arduino.h>
struct ReceiverChannels { float roll, pitch, throttle, yaw, aux; bool valid; };
bool initReceiver();
bool readReceiver(ReceiverChannels& channels);
bool receiverSignalValid();
