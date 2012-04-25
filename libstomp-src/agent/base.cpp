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

	int                                                               
		die (int exitCode, const char *message, apr_status_t reason)              
		{                                                                         
			char msgbuf[80];                                                  
			apr_strerror (reason, msgbuf, sizeof (msgbuf));                   
			fprintf (stderr, "%s: %s (%d)\n", message, msgbuf, reason);       
			exit (exitCode);                                                  
			return reason;                                                    
		}                                                                         


	string BaseAgent::agentName() {     
		//std::string name = "base";  
		//return name;                   
	};                                     

	BaseAgent::BaseAgent () {
	}

	void BaseAgent::init (stomp_connection * connection,
			apr_pool_t * pool)
	{
		apr_status_t rc;


		rc = apr_pool_create (&_pool, NULL);                              
		rc == APR_SUCCESS || die (-2, "Could not allocate pool", rc);    

		fprintf (stdout, "Connecting to pool....");                            
		rc = stomp_connect (&_connection, "127.0.0.1", 6163, _pool);       
		rc == APR_SUCCESS || die (-2, "Could not connect\n", rc);          
		fprintf (stdout, "OK\n");                                        

		name=agentName();

		fprintf (stdout, "Logging in ....");                                                          
		{                                                                                                      
			stomp_frame frame;                                                                             
			char command[] = "CONNECT";
			frame.command = command;                                                                     
			frame.headers = apr_hash_make (_pool);                                                          
			apr_hash_set (frame.headers, "login", APR_HASH_KEY_STRING,                                     
					"mcollective");                                                                
			apr_hash_set (frame.headers, "passcode", APR_HASH_KEY_STRING,                                  
					"marionette");                                                                 
			frame.body = NULL;                                                                             
			frame.body_length = -1;                                                                        
			rc = stomp_write (_connection, &frame, _pool);                                                   
			rc == APR_SUCCESS || die (-2, "Could not send frame", rc);                                     
		}                                                                                                      
		fprintf (stdout, "OK\n");                                        


		fprintf(stdout, "Sending Subscribe: ");                                                                 
		{                                                                                                      

			stomp_frame frame;                                                                             

			char command[] = "SUB";
			frame.command = command;                                                                         

			frame.headers = apr_hash_make(_pool);                                                           
			std::string topic= "/topic/mcollective."+name+".command";
			std::cout << topic << std::endl;
			apr_hash_set(frame.headers, "destination", APR_HASH_KEY_STRING, topic.c_str());
			frame.body_length = -1;                                                                        

			frame.body = NULL;                                                                             

			rc = stomp_write(_connection, &frame, _pool);                                                    

			rc==APR_SUCCESS || die(-2, "Could not send frame", rc);                                        

		}                                                                                                      

	};


	void BaseAgent::reply(string requestid,YAML::Emitter *reply_message_body_yaml) {

		std::cout << "[Reply-"+name+"-"+requestid+"] Replying to Request-Id" << requestid << std::endl;
		std::string reply_message_body = (*reply_message_body_yaml).c_str ();
		std::cout << "[Reply-"+name+"-"+requestid+"] Body:\n" ;
		cout << "******\n";
		cout << reply_message_body << std::endl;
		cout << "******\n";

		// Make the body a valid YAML Doc
		std::string yaml_message_body = "--- ";
		yaml_message_body=yaml_message_body+reply_message_body.c_str() +"\n";
		std::cout << "[Reply-"+name+"-"+requestid+"] Body YAML:\n";
		cout << "******\n";
		cout << yaml_message_body << std::endl;
		cout << "******\n";

		// Append the PSK to calculate the MD5	
		std::string body_psk = yaml_message_body;
		std::string psk = "unset";
		body_psk.append (psk);

		/////////// MD5 sign body
		// MD5 -  https://gist.github.com/2389719
		// but needs a real string
		// http://social.msdn.microsoft.com/Forums/en/Vsexpressvc/thread/e1774395-ba99-4fe6-98eb-2224a67984b9
		std::stringstream md5sumstream;
		unsigned char md5_result[MD5_DIGEST_LENGTH];
		const unsigned char *constStr =
			reinterpret_cast < const unsigned char *>(body_psk.c_str ());
		MD5 (constStr, body_psk.length (), md5_result);
		for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
		{
			//printf ("%02x", md5_result[i]);
			char digit[2];
			sprintf (digit, "%02x", md5_result[i]);
			md5sumstream << digit;
		}
		printf ("\n");


		std::string hash = md5sumstream.str();
		std::cout << "[Reply-"+name+"-"+requestid+"] Md5 hash:" << hash << std::endl;

		//SSL
		//hash = "H1OjPrMykjzQKuL+H4G1zqjBbfypFMsUZ4twkSaMKQ/9KE7eOiK15SADsRuW\n/MZvxr5+sIEn5vliHSkTSvegHfrP5pPva2TUPK281+R8UZAi8NX1TrUwOUCy\nY3qxRFWcYkUCtPMKmz6U6A52Ikym5rndKHRfLM9KydjFRPieCaQ=\n";

		// Our reply topic
		std::string topic= "/topic/mcollective."+name+".reply";

		// Construct answer

		YAML::Emitter reply_message_yaml;

		reply_message_yaml << YAML::BeginMap;
		reply_message_yaml << YAML::Key << ":msgtime";
		reply_message_yaml << YAML::Value << 1334988601;
		reply_message_yaml << YAML::Key << ":requestid";
		reply_message_yaml << YAML::Value << requestid;
		reply_message_yaml << YAML::Key << ":body";
		reply_message_yaml << YAML::Value << YAML::Literal << yaml_message_body;
		reply_message_yaml << YAML::Key << ":senderid";
		reply_message_yaml << YAML::Value << "mcpp";
		reply_message_yaml << YAML::Key << ":senderagent";
		reply_message_yaml << YAML::Value << name;
		reply_message_yaml << YAML::Key << ":msgtarget";
		reply_message_yaml << YAML::Value << topic;
		reply_message_yaml << YAML::Key << ":hash";
		reply_message_yaml << YAML::Value << YAML::Literal <<  hash;
		reply_message_yaml << YAML::EndMap;

		// Put it in a string
		std::string reply_message = reply_message_yaml.c_str ();
		std::cout << "[Reply-"+name+"-"+requestid+"] Reply Message:\n" ;
		cout << "******\n";
		std::cout<< reply_message << std::endl;
		cout << "******\n";

		///Send it

		stomp_frame reply_frame;
		char command[] = "SEND";
		reply_frame.command = command;
		reply_frame.headers = apr_hash_make (_pool);
		apr_hash_set (reply_frame.headers, "destination", APR_HASH_KEY_STRING,
				topic.c_str());

		reply_frame.body_length = -1;
		char *caution = const_cast<char *>(reply_message.c_str());

		reply_frame.body = caution;
		apr_status_t rc;
		rc = stomp_write (_connection, &reply_frame, _pool);
		//rc == APR_SUCCESS || die (-2, "Could not send frame", rc);

	};

	void BaseAgent::handle (stomp_frame * frame)
	{
		fprintf (stdout, "Received: %s, %s\n", frame->command, frame->body);
		std::string msg (frame->body);
		if (strlen (frame->body) == 0)
		{
			cout << "No body in packet" << std::endl;
			return;
		}

		std::stringstream msg_stream (msg);
		std::cout << "[Request-"+name +"] Recieved Packet" << std::endl;
		cout << "******\n";
		cout << msg_stream.str ();
		cout << "******\n";

		//Parse msg stream
		YAML::Parser msg_parser (msg_stream);
		YAML::Node msg_doc;
		msg_parser.GetNextDocument (msg_doc);

		for (YAML::Iterator it = msg_doc.begin (); it != msg_doc.end (); ++it)
		{
			std::string key, value;
			it.first () >> key;
			//std::cout << "Key: " << key << std::endl;
		}


		// Body seems to be multiline string of yaml                                          
		// Parsing strings http://stackoverflow.com/questions/2813030/yaml-cpp-parsing-strings
		std::string body;
		msg_doc[":body"] >> body;

		std::stringstream body_stream (body);
		std::cout << "[Request-"+name +"] Recieved Body" << std::endl;
		cout << "******\n";
		cout << body_stream.str ();
		cout << "******\n";
		YAML::Parser body_parser (body_stream);
		YAML::Node body_doc;
		std::string action;
		body_parser.GetNextDocument (body_doc);
		//body_doc >> action;
		//std::cout << action;
		receive(&msg_doc,&body_doc);



	};


	void BaseAgent::receive(YAML::Node *msg_doc, YAML::Node *body_doc) {   
	};                                                                      

	void BaseAgent::start() {
		while (1)                                                             
		{                                                                   
			fprintf (stdout, "Listening for messages\n");                            
			{                                                                 
				stomp_frame *frame;                                             
				int rc = stomp_read (_connection, &frame, _pool);                     
				rc == APR_SUCCESS || die (-2, "Could not read frame", rc);      
				this->handle(frame);                                 
			}                                                                 
		}                                                                   
	};

}

