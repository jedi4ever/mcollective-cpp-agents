#include "stomp.h"
#include "base.h"
#include "yaml.h"

namespace Mcollective
{
	class DiscoveryAgent : public BaseAgent {

		public: DiscoveryAgent ();

		public: std::string agentName();
		public: void receive(YAML::Node *msg_doc, YAML::Node *body_doc);

	};
}

