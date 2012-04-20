#include "discovery.h"
#include "stomp.h"
#include "yaml.h"
#include <openssl/md5.h>

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

#include <pthread.h>              
                                  
#include <stdio.h>                
#include <stdlib.h>               
#include <string.h>               

using namespace std;

namespace Mcollective
{

	DiscoveryAgent::DiscoveryAgent (): BaseAgent::BaseAgent()
	{
	};

	string DiscoveryAgent::agentName() {
		std::string name = "discovery";
		return name;
	}

	void DiscoveryAgent::receive(YAML::Node *msg_doc, YAML::Node *body_doc) 
	{
		printf("yes we are receiving");
		std::string requestid;
		std::string senderid;
		std::string msgtarget;
		(*msg_doc)[":msgtarget"] >> msgtarget;
		(*msg_doc)[":requestid"] >> requestid;
		(*msg_doc)[":senderid"] >> senderid;
		
//		std::cout << "We received requesid" << requestid;
		/////////////  Construct body
		// Construct YAML body
		YAML::Emitter reply_message_body_yaml;
		reply_message_body_yaml << "pong";
		reply(requestid,&reply_message_body_yaml);
	};


}

