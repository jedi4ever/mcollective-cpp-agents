#include "stomp.h"

namespace Mcollective
{
  class DiscoveryAgent
  {
    stomp_connection *_connection;
    apr_pool_t *_pool;

  public:
      DiscoveryAgent (stomp_connection *, apr_pool_t *);

    virtual void handle (stomp_frame *);
  };
}
