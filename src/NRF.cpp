#include "NRF.h"

// Initialize the radio and addresses
RF24 radio(49, 4); // CE, CSN
uint8_t address[][6] = {"1Node", "2Node"};
bool radioNumber = 0;
unsigned long lastSendTime = 0;

unsigned long rc_send_interval = 50;

// Initialize the data packages
RC_Data_Package rc_data;
Hexapod_Data_Package hex_data;

void setupNRF() {
    radio.begin();
    radio.setPALevel(RF24_PA_HIGH);
    radio.setPayloadSize(sizeof(rc_data));
    radio.setChannel(112);
    radio.enableAckPayload();
    radio.setRetries(5, 5);
    radio.openWritingPipe(address[radioNumber]);

    rc_data.joy1_X = 127;
    rc_data.joy1_Y = 127;
    rc_data.joy2_X = 127;
    rc_data.joy2_Y = 127;
    rc_data.toggle1 = 0;
    rc_data.toggle2 = 0;
    rc_data.toggle3 = 0;
    rc_data.toggle4 = 0;

}

void sendNRFData() {
    if (millis() - lastSendTime >= rc_send_interval) {
        lastSendTime = millis();
        bool report = radio.write(&rc_data, sizeof(rc_data));

        if (report) {
            Serial.println("Data sent successfully");

            if (radio.isAckPayloadAvailable()) {
                radio.read(&hex_data, sizeof(hex_data));
                Serial.print("Received ack payload: ");
                Serial.println(hex_data.current_sensor_value);
            }
        } else {
            Serial.println("Failed to send data");
        }
    }
}