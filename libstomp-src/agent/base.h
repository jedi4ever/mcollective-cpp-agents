#include <yaml.h>

namespace Mcollective                                     
{                                                         
	class BaseAgent {               
		public: stomp_connection *_connection;                        
		public: apr_pool_t *_pool;                                    

		public:                                                 
		BaseAgent (stomp_connection *, apr_pool_t *);    

		virtual void handle (stomp_frame *);                  
		public: virtual void start();                                 

	     void receive(YAML::Node *msg_doc, YAML::Node *body_doc);                             
	     void reply(std::string request_id, YAML::Emitter *reply_message_yaml);               
	};                                                      
}                                                         
