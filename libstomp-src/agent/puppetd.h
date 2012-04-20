#include "stomp.h"
#include "base.h"
#include "yaml.h"

namespace Mcollective
{
	class PuppetdAgent : public BaseAgent {

		public: PuppetdAgent ();

	  public: std::string agentName();

			public: void receive(YAML::Node *msg_doc, YAML::Node *body_doc);

	};
}

