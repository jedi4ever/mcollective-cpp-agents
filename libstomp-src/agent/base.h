#include <yaml.h>

#ifndef __MCOLLECTIVEBASEAGENT__ 
#define __MCOLLECTIVEBASEAGENT__ 

namespace Mcollective                                     
{                                                         
	class BaseAgent {               
		public: stomp_connection *_connection;                        
		public: apr_pool_t *_pool;                                    
		public: std::string name;

		public:                                                 
		BaseAgent ();

		virtual void handle (stomp_frame *);                  
		public: virtual void start();                                 
		virtual void init (stomp_connection *, apr_pool_t *);    

	     public: virtual std::string agentName();
	     virtual void receive(YAML::Node *msg_doc, YAML::Node *body_doc);                             
	     virtual void reply(std::string request_id, YAML::Emitter *reply_message_yaml);               
	};                                                      
}                                                         

#endif
