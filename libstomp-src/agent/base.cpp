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

	BaseAgent::BaseAgent (stomp_connection * connection,
			apr_pool_t * pool)
	{
		//_pool =  pool;
		//_connection = connection;
		//
		apr_status_t rc;


		rc = apr_pool_create (&_pool, NULL);                              
		rc == APR_SUCCESS || die (-2, "Could not allocate pool", rc);    

		fprintf (stdout, "Connecting......");                            
		rc = stomp_connect (&_connection, "127.0.0.1", 6163, _pool);       
		rc == APR_SUCCESS || die (-2, "Could not connect", rc);          
		fprintf (stdout, "OK\n");                                        



		fprintf (stdout, "Sending connect message.");                                                          
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

		fprintf(stdout, "Sending Subscribe.");                                                                 
		{                                                                                                      

			stomp_frame frame;                                                                             

			char command[] = "SUB";
			frame.command = command;                                                                         

			frame.headers = apr_hash_make(_pool);                                                           

			apr_hash_set(frame.headers, "destination", APR_HASH_KEY_STRING, "/topic/mcollective.discovery.command");                                                                                                              
			frame.body_length = -1;                                                                        

			frame.body = NULL;                                                                             

			rc = stomp_write(_connection, &frame, _pool);                                                    

			rc==APR_SUCCESS || die(-2, "Could not send frame", rc);                                        

		}                                                                                                      

	};


	void BaseAgent::reply(string requestid,YAML::Emitter *reply_message_body_yaml) {
		/////////// MD5 sign body
		std::string reply_message_body = reply_message_body_yaml->c_str ();
		std::cout << reply_message_body << std::endl;
		// Append PSK to it
		std::string psk = "unset";
		std::string body_psk = reply_message_body;
		body_psk.append (psk);
		std::stringstream md5sumstream;

		// MD5 -  https://gist.github.com/2389719
		// but needs a real string
		// http://social.msdn.microsoft.com/Forums/en/Vsexpressvc/thread/e1774395-ba99-4fe6-98eb-2224a67984b9
		unsigned char md5_result[MD5_DIGEST_LENGTH];
		const unsigned char *constStr =
			reinterpret_cast < const unsigned char *>(body_psk.c_str ());
		MD5 (constStr, body_psk.length (), md5_result);
		for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
		{
			printf ("%02x", md5_result[i]);
			char digit[2];
			sprintf (digit, "%02x", md5_result[i]);
			md5sumstream << digit;
		}
		printf ("\n");


		std::cout << "md5 stream:" << md5sumstream.str () << std::endl;
		std::string hash = md5sumstream.str ();
		std::cout << "hash:" << hash << std::endl;

		// Construct answer

		YAML::Emitter reply_message_yaml;

		reply_message_yaml << YAML::BeginMap;
		reply_message_yaml << YAML::Key << ":msgtime";
		reply_message_yaml << YAML::Value << 1010101;
		reply_message_yaml << YAML::Key << ":requestid";
		reply_message_yaml << YAML::Value << requestid;
		reply_message_yaml << YAML::Key << ":body";
		reply_message_yaml << YAML::Value << reply_message_body;
		reply_message_yaml << YAML::Key << ":senderid";
		reply_message_yaml << YAML::Value << "mcpp";
		reply_message_yaml << YAML::Key << ":senderagent";
		reply_message_yaml << YAML::Value << "discovery";
		reply_message_yaml << YAML::Key << ":msgtarget";
		reply_message_yaml << YAML::Value << "/topic/mcollective.discovery.reply";

		reply_message_yaml << YAML::Key << ":hash";
		reply_message_yaml << YAML::Value << hash;
		reply_message_yaml << YAML::EndMap;


		// Put it in a string
		std::string reply_message = reply_message_yaml.c_str ();
		cout << reply_message;

		///Send it

		stomp_frame reply_frame;
		char command[] = "SEND";
		reply_frame.command = command;
		reply_frame.headers = apr_hash_make (_pool);
		apr_hash_set (reply_frame.headers, "destination", APR_HASH_KEY_STRING,
				"/topic/mcollective.discovery.reply");

		reply_frame.body_length = -1;
		char *caution = const_cast<char *>(reply_message.c_str());

		reply_frame.body = caution;
		apr_status_t rc;
		rc = stomp_write (_connection, &reply_frame, _pool);
		//rc == APR_SUCCESS || die (-2, "Could not send frame", rc);

	};

	void BaseAgent::handle (stomp_frame * frame)
	{
		fprintf (stdout, "Response: %s, %s\n", frame->command, frame->body);
		std::string msg (frame->body);
		if (strlen (frame->body) == 0)
		{
			cout << "No body in packet";
			return;
		}

		std::stringstream msg_stream (msg);
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
		cout << "******\n";
		cout << body_stream.str ();
		cout << "******\n";
		YAML::Parser body_parser (body_stream);
		YAML::Node body_doc;
		std::string action;
		body_parser.GetNextDocument (body_doc);
		body_doc >> action;
		std::cout << action;
		receive(&msg_doc,&body_doc);



	};

	 void BaseAgent::receive(YAML::Node *msg_doc, YAML::Node *body_doc) {
	};

	void BaseAgent::start() {
		printf("lalalaal");
		while (1)                                                             
		{                                                                   
			fprintf (stdout, "Reading Response.");                            
			{                                                                 
				stomp_frame *frame;                                             
				int rc = stomp_read (_connection, &frame, _pool);                     
				rc == APR_SUCCESS || die (-2, "Could not read frame", rc);      
				fprintf (stdout, "OK\n");                                     
				this->handle(frame);                                 
			}                                                                 
		}                                                                   
	};

}

