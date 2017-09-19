#include <string.h>
#include <stdio.h>
#include "mqtt_transport.h"

#include "UID_message.h"

int parse_result(uint8_t *buffer, size_t size, UID_ClientChannelCtx *ctx, char *res, size_t rsize, int64_t id)
{
	BTC_Address sender;
	int error = 0;
	int64_t sID;

	int ret = UID_parseRespMsg(buffer, size, sender, sizeof(sender), &error, res, rsize, &sID);
	if ( ret ) return ret;
	if (error) return UID_MSG_RPC_ERROR | error;
	if (strcmp(sender, ctx->peerid)) return UID_MSG_INVALID_SENDER;
	if (sID != id) return UID_MSG_ID_MISMATCH;
	return 0;
}

void RPC_request(void)
{
	int ret;
	int method = 31;
	char Machine_name[UID_NAME_LENGHT] = "demo0123456789";
	char parameter_to_the_method[50] = {0};

	// get the command request from stdinput
	printf("\n\n------------ enter request (es. demo0123456789 33 {\"hello\":\"world\"} -----------------\n\n");
	#define _STR(a) #a
	#define STR(a) _STR(a)
	if (3 != scanf("%" STR(UID_NAME_LENGHT) "s %d %50s", Machine_name, &method, parameter_to_the_method))
		return; // not a valid request

	// create the contest for the communication (contract, identities of the peers, etc)
	UID_ClientChannelCtx ctx;
	if ( UID_MSG_OK != (ret = UID_createChannel(Machine_name, &ctx)) ) {

//			< manage_error(ret) >
		printf("Error! - UID_createChannel(%s) return %d\n", Machine_name, ret);

		return;
	}

	// format the message
	uint8_t buffer[1024];
	size_t size = sizeof(buffer);
	int64_t id;
	if ( UID_MSG_OK != (ret = UID_formatReqMsg(ctx.myid, method, parameter_to_the_method, buffer, &size, &id)) ) {

//			< manage_error(ret) >
		printf("Error! - UID_formatReqMsg() return %d\n", ret);

		return;
	}

//		< Send_Msg_to_provider(buffer, size) >
	mqttUserSendMsg(Machine_name, ctx.myid, buffer, size - 1);

	uint8_t *msg = NULL;

//		< Wait_for_Msg_from_provider(msg, &size) >
	mqttUserWaitMsg(&msg, &size);

	// parse the received message
	char result[1024] = "";
	if ( UID_MSG_OK != (ret = parse_result(msg, size, &ctx, result, sizeof(result), id))) {

//			< manage_error(ret) >
		printf("Error! - parse_result() return %d\n", ret);

		return;
	}

	free(msg);

//  now you have the <result> from the execution on the provider
//  of the requested method: you can use it as you need

	// close the channel
	UID_closeChannel(&ctx);
}
