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

	DiscoveryAgent::DiscoveryAgent (stomp_connection * connection,
			apr_pool_t * pool): BaseAgent::BaseAgent(connection,pool)
	{
	};

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
		reply_message_body_yaml << "pong";
		reply(requestid,&reply_message_body_yaml);
	};


}

