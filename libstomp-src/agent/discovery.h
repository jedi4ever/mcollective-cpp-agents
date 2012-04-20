#include "stomp.h"
#include "base.h"
#include "yaml.h"

namespace Mcollective
{
  class DiscoveryAgent : public BaseAgent {

  public:
    DiscoveryAgent (stomp_connection *, apr_pool_t *);

    virtual void handle (stomp_frame *);
	void receive(YAML::Node *msg_doc, YAML::Node *body_doc);
	void reply(std::string request_id, YAML::Emitter *reply_message_yaml);

  };
}

