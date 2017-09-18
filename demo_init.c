#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "mqtt_transport.h"



#include "UID_bchainBTC.h"
#include "UID_identity.h"

// Update Cache Thread
// gets contracts from the BlockChain and updates the local cache
void *updateCache(void *arg)
{
	cache_buffer *cache;
	int ret;

	while(1)
	{
		ret = UID_getContracts(&cache);

//		< manage cache persistence if needed >

		sleep(60);
	}
	return NULL;
	(void)arg;(void)ret;
}

int main(void)
{
	pthread_t thr;
	char imprinting[1024];

	char Entity_NAME[15] = {0};
	// generate or load the Entity Name
    {   // try to read serial.no
		uint8_t serial[5] = {0};
        int uniq = open("serial.no", O_RDWR|O_CREAT, 0644);
        if (read(uniq, serial, sizeof(serial)) != sizeof(serial)) // if we cant read userial.no generate one
        {
            int rnd = open("/dev/random", O_RDONLY);
            if(read(rnd, serial, sizeof(serial)) <= 0) // if we cant read /dev/random use time for seed
                *(int32_t *)serial = time(NULL);
            close(rnd);
            write(uniq, serial, sizeof(serial));
        }
        close(uniq);
        snprintf(Entity_NAME, sizeof(Entity_NAME), "demo%02x%02x%02x%02x%02x",serial[0], serial[1], serial[2], serial[3], serial[4]);
    }
    // start the mqttWorker thread
    strncpy(mqtt_address, "tcp://appliance4.uniquid.co:1883",sizeof(mqtt_address));
    pthread_create(&thr, NULL, mqttWorker, Entity_NAME);

	// set up the URL to insight-api appliance
	strcpy(UID_appliance, "http://explorer.uniquid.co:3001/insight-api");
	// generate or load the identity
	UID_getLocalIdentity(NULL);
	// build the imprinting string
	snprintf(imprinting, sizeof(imprinting), "{\"name\":\"%s\",\"xpub\":\"%s\"}", "Entity NAME", UID_getTpub());

//	< manage imprinting information >

	// start the the thread that updates the
	// contracts cache from the block-chain
	// here we are using pthread, but is up to the user of the library
	// to chose how to schedule the execution of UID_getContracts()
	pthread_create(&thr, NULL, updateCache, NULL);

//		...
}
