#include "stomp.h"
#include "base.h"
#include "yaml.h"

namespace Mcollective
{
	class DiscoveryAgent : public BaseAgent {

		public:
			DiscoveryAgent (stomp_connection *, apr_pool_t *);

			void receive(YAML::Node *msg_doc, YAML::Node *body_doc);

	};
}

