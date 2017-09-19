# uidcore-C-demo
Demo code for uidcore-c library

Here are the steps to setup and build a minimal demo project.<br>
The uidcore-c library is at the moment supported only under Linux
and it needs the following libraries available on the system:
- pthread
- curl

The demo implements a very simple Uniquid Node featuring Provider and User functionalities.
As a Provider it implements a simple echo function: user-defined RPC 33.
As a user it can request a RPC execution with the following syntax from command line:<br>
```
ProviderName method ParameterString
Es.
demo0123456789 33 {"hello":"world"}
```

In this demo we provide an implementation of a transport for the Uniquid messages based on MQTT

To set up the demo you can simply clone it from github
```
git clone --recurse-submodules git@github.com:uniquid/uidcore-C-demo.git
cd uidcore-C-demo
# build the libraries
make -C uidcore-c
make -C paho.mqtt.c
# build the demo
make
```
else you can follow this steps (the commits of this repository will guide you through these steps)
```
# make the demo directory
mkdir uidcore-C-demo
# cd into it
cd uidcore-C-demo
# and init the git repository
git init
```
clone the uidcore-c library @ commit bb4afc...2d9c34 and add it as submodule
```
git clone git@github.com:uniquid/uidcore-c.git
cd uidcore-c/
git checkout bb4afc10313583cba7a396af470400a75f2d9c34
git submodule init
git submodule update --recursive
cd ..
git submodule add git@github.com:uniquid/uidcore-c.git
```
copy the example code from uidcore-c
```
cp uidcore-c/example_init.c demo_init.c
cp uidcore-c/example_provider.c demo_provider.c
cp uidcore-c/example_user.c demo_user.c
```
in this demo we will use MQTT for the transport channel and for the imprinting.<br>
mqtt_transport.c and mqtt_transport.h provide the implementation.
the following four functions are the interface to the engine:
- int mqttUserWaitMsg(uint8_t **msg, size_t *len);
- int mqttProviderWaitMsg(uint8_t **msg, size_t *len);
- int mqttUserSendMsg(char *send_topic, char *recv_topic, uint8_t *msg, size_t size);
- int mqttProviderSendMsg(char *send_topic, uint8_t *msg, size_t size);

paho.mqtt.c library is a prerequisite

clone the paho.mqtt.c library @ commit 6a8009...acb2fb and add it as submodule
```
git clone git@github.com:uniquid/paho.mqtt.c.git
cd paho.mqtt.c/
git checkout 6a800988c5b6f6fef9b72c35249bdffe8eacb2fb
cd ..
git submodule add git@github.com:uniquid/paho.mqtt.c.git
```
Add to main() the code to generate the Entity Name
```
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
```
Add to main() the code to start the mqttWorker thread
```
    strncpy(mqtt_address, "tcp://appliance4.uniquid.co:1883",sizeof(mqtt_address));
    pthread_create(&thr, NULL, mqttWorker, Entity_NAME);
```
Add to main() the code to manage imprinting information
```
	mqttProviderSendMsg("UID/announce", (uint8_t *)imprinting, strlen(imprinting));
	printf("%s\n", imprinting);
```
Add to main() the code to start the "provider" thread
```
	void* service_provider(void *arg);
	pthread_create(&thr, NULL, service_provider, NULL);
```
Implement the "Wait_for_Msg_from_user" and "Send_Msg_to_user" in provider code
```
//		< Wait_for_Msg_from_user(msg, &size) >
		mqttProviderWaitMsg(&msg, &size);
		      .....
		free(msg);
```
```
//		< Send_Msg_to_user(response, respsize - 1) >
		mqttProviderSendMsg(sctx.contract.serviceUserAddress, response, respsize - 1);
```
Add some error handling in the provider code
```
//			< manage_error(ret) >
			printf("Error! - UID_accept_channel() return %d\n", ret);
```
```
//			< manage_error(ret) >
			printf("Error! - UID_perform_request() return %d\n", ret);
```
In demo_user.c add the code to read the RPC request from the command line
```
	char Machine_name[UID_NAME_LENGHT] = "demo0123456789";
	char parameter_to_the_method[50] = {0};

	// get the command request from stdinput
	printf("\n\n------------ enter request (es. demo0123456789 33 {\"hello\":\"world\"} -----------------\n\n");
	#define _STR(a) #a
	#define STR(a) _STR(a)
	if (3 != scanf("%" STR(UID_NAME_LENGHT) "s %d %50s", Machine_name, &method, parameter_to_the_method))
		return; // not a valid request
```
Implement the "Send_Msg_to_provider" and "Wait_for_Msg_from_provider" in user code
```
//		< Send_Msg_to_provider(buffer, size) >
	mqttUserSendMsg(Machine_name, ctx.myid, buffer, size - 1);
```
```
//		< Wait_for_Msg_from_provider(msg, &size) >
	mqttUserWaitMsg(&msg, &size);
	      .....
	free(msg);
```
Add some error handling in the user code
```
//			< manage_error(ret) >
		printf("Error! - UID_createChannel(%s) return %d\n", Machine_name, ret);
```
```
//			< manage_error(ret) >
		printf("Error! - UID_formatReqMsg() return %d\n", ret);
```
```
//			< manage_error(ret) >
		printf("Error! - parse_result() return %d\n", ret);
```
use the RPC result: we just print it
```
//  now you have the <result> from the execution on the provider
//  of the requested method: you can use it as you need
	printf("RPC method %d successfully executed\nreturned <%s>\n", method, result);
```
call RPC_request() from main()
```
#define _EVER_ ;;
	void RPC_request(void);
	for(_EVER_) RPC_request(); // perform user request
```
add some debugging code to print the contracts at every update
```
		// just print the cache contents
		printCache(cache);
```
