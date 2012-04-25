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
		std::string requestid;
		std::string senderid;
		std::string msgtarget;
		(*msg_doc)[":msgtarget"] >> msgtarget;
		(*msg_doc)[":requestid"] >> requestid;
		(*msg_doc)[":senderid"] >> senderid;
		
		/////////////  Construct body
		// Construct YAML body
		YAML::Emitter reply_message_body_yaml;
		//PSK
		reply_message_body_yaml << "pong";
		//SSL
		//reply_message_body_yaml << "--- pong";
		reply(requestid,&reply_message_body_yaml);
	};


}

