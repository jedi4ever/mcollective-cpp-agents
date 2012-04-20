#include "puppetd.h"
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

	PuppetdAgent::PuppetdAgent (): BaseAgent::BaseAgent()
	{
	};

	  string PuppetdAgent::agentName() {                    
		  std::string name = "puppetd"; 
		  return name;                    
	  };
	void PuppetdAgent::receive(YAML::Node *msg_doc, YAML::Node *body_doc) 
	{
		printf("yes we are receiving");
		std::string requestid;
		std::string senderid;
		std::string msgtarget;
		(*msg_doc)[":msgtarget"] >> msgtarget;
		(*msg_doc)[":requestid"] >> requestid;
		(*msg_doc)[":senderid"] >> senderid;
		
		/////////////  Construct body
		// Construct YAML body

		YAML::Emitter data;
		data << YAML::BeginMap;
		data << YAML::Key << ":status";
		data << YAML::Value << "running";
		data << YAML::Key << ":running";
		data << YAML::Value << "1";
		data << YAML::Key << ":disabled";
		data << YAML::Value << "0";
		data << YAML::Key << ":idling";
		data << YAML::Value << "0";
		data << YAML::Key << ":stopped";
		data << YAML::Value << "0";
		data << YAML::Key << ":lastrun";
		data << YAML::Value << "0";
		data << YAML::Key << ":output";
		data << YAML::Value << "Houston we got a lift off!";
		data << YAML::EndMap;

		YAML::Emitter r;
		r << YAML::BeginMap;
		r << YAML::Key << ":statuscode";
		r << YAML::Value << "0";
		r << YAML::Key << ":statusmsg";
		r << YAML::Value << "Houston we got a lift off!";
		r << YAML::Key << ":data";
		r << YAML::Value << data.c_str();
		r << YAML::EndMap;

		
		reply(requestid,&r);
	};


}

/*
       reply[:status]  = puppet_daemon_status                                                                          
       reply[:running] = reply[:status] == 'running'  ? 1 : 0                                                          
       reply[:enabled] = reply[:status] == 'disabled' ? 0 : 1                                                          
       reply[:idling]  = reply[:status] == 'idling'   ? 1 : 0                                                          
       reply[:stopped] = reply[:status] == 'stopped'  ? 1 : 0                                                          
       reply[:lastrun] = 0                                                                                             
       reply[:lastrun] = File.stat(@statefile).mtime.to_i if File.exists?(@statefile)                                  
       reply[:output]  = "Currently #{reply[:status]}; last completed run #{Time.now.to_i - reply[:lastrun]} seconds ag
"                                                                                                                      
     end                                                                                                               
*/
