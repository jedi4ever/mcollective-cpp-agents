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


	void BaseAgent::handle (stomp_frame *) {
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

