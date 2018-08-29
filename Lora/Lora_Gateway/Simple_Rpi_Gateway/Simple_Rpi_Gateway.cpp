// rf95_server.cpp

#include "bcm2835.h"
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include <RH_RF95.h>


// https://github.com/asheeshgoja/Lora_Raspberrypi
// see https://github.com/asheeshgoja/Lora_Raspberrypi
#define BOARD_LORASPI


// Now we include RasPi_Boards.h so this will expose defined 
// constants with CS/IRQ/RESET/on board LED pins definition
#include "../RasPiBoards.h"

// Our RFM95 Configuration 
#define RF_FREQUENCY  915.00
#define RF_NODE_ID    1

// Create an instance of a driver
//RH_RF95 rf95(RF_CS_PIN, RF_IRQ_PIN);
RH_RF95 rf95(RF_CS_PIN);

//Flag for Ctrl-C
volatile sig_atomic_t force_exit = false;

void sig_handler(int sig)
{
  printf("\n%s Break received, exiting!\n", __BASEFILE__);
  force_exit=true;
}

//Main Function
int main(int argc, const char* argv[])
{
	unsigned long led_blink = 0;

	signal(SIGINT, sig_handler);
	printf("%s\n", __BASEFILE__);

	if (!bcm2835_init()) {
		fprintf(stderr, "%s bcm2835_init() Failed\n\n", __BASEFILE__);
		return 1;
	}

	printf("RF95 CS=GPIO%d", RF_CS_PIN);


	if (!rf95.init()) {
		fprintf(stderr, "\nRF95 module init failed, Please verify wiring/module\n");
	}
	else {

		// RF95 Modules don't have RFO pin connected, so just use PA_BOOST
		// check your country max power useable, in EU it's +14dB

		// You can optionally require this module to wait until Channel Activity
		// Detection shows no activity on the channel before transmitting by setting
		// the CAD timeout to non-zero:
		//rf95.setCADTimeout(10000);

		// Adjust Frequency
		rf95.setFrequency(RF_FREQUENCY);

		// If we need to send something
	   // rf95.setThisAddress(RF_NODE_ID);
	   // rf95.setHeaderFrom(RF_NODE_ID);

		// Be sure to grab all node packet 
		// we're sniffing to display, it's a demo
		rf95.setPromiscuous(true);

		rf95.setTxPower(14, false);

		// We're ready to listen for incoming message
		//rf95.setModeTx();
		//rf95.setModeRx();

		printf(" OK NodeID=%d @ %3.2fMHz\n", RF_NODE_ID, RF_FREQUENCY);
		printf("Listening packet...\n");

		//Begin the main body of code
		while (!force_exit) {

			if (rf95.available()) {

				// Should be a message for us now
				uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
				uint8_t len = sizeof(buf);
				uint8_t from = rf95.headerFrom();
				uint8_t to = rf95.headerTo();
				uint8_t id = rf95.headerId();
				uint8_t flags = rf95.headerFlags();;
				int8_t rssi = rf95.lastRssi();

				if (rf95.recv(buf, &len)) {
					printf("Packet[%02d] #%d => #%d %ddB: ", len, from, to, rssi);
					//    printbuffer(buf, len);
					printf("Msg = %s", (char*)buf);

					//send response
					uint8_t data[20] = "";
			 		sprintf((char*)data,"Ack for %d" , id);
					uint8_t len = sizeof(data);
					printf("Sending ack");
					printf("\n");
					rf95.send(data, len);
					rf95.waitPacketSent();

					delay(100);

				}
				else {
					Serial.print("receive failed");
				}
				printf("\n");
			}


			bcm2835_delay(5);
		}
	}

	printf("\n%s Ending\n", __BASEFILE__);
	bcm2835_close();
	return 0;
}
