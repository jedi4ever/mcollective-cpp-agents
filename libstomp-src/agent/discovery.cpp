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

	void DiscoveryAgent::handle (stomp_frame * frame)
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


		std::string requestid;
		std::string senderid;
		std::string msgtarget;
		msg_doc[":msgtarget"] >> msgtarget;
		msg_doc[":requestid"] >> requestid;
		msg_doc[":senderid"] >> senderid;

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

		/////////////  Construct body
		// Construct YAML body
		YAML::Emitter reply_message_body_yaml;
		reply_message_body_yaml << "pong";

		/////////// MD5 sign body
		std::string reply_message_body = reply_message_body_yaml.c_str ();
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

}

