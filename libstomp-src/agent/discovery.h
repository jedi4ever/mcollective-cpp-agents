#include "stomp.h"
#include "base.h"

namespace Mcollective
{
  class DiscoveryAgent : public BaseAgent {

  public:
    DiscoveryAgent (stomp_connection *, apr_pool_t *);

    virtual void handle (stomp_frame *);
  };
}

