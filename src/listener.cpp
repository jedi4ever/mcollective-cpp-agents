#include "agent/discovery.h"
#include "listener.h"
#include "stomp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

using namespace std;

int
die (int exitCode, const char *message, apr_status_t reason)
{
  char msgbuf[80];
  apr_strerror (reason, msgbuf, sizeof (msgbuf));
  fprintf (stderr, "%s: %s (%d)\n", message, msgbuf, reason);
  exit (exitCode);
  return reason;
}

namespace Mcollective
{
  Listener::Listener ()
  {
  }


  void Listener::DoSomething ()
  {

    apr_status_t rc;
    apr_pool_t *pool;
    stomp_connection *connection;

    setbuf (stdout, NULL);

    rc = apr_initialize ();
    rc == APR_SUCCESS || die (-2, "Could not initialize", rc);
    atexit (terminate);

    rc = apr_pool_create (&pool, NULL);
    rc == APR_SUCCESS || die (-2, "Could not allocate pool", rc);

    fprintf (stdout, "Connecting......");
    rc = stomp_connect (&connection, "127.0.0.1", 6163, pool);
    rc == APR_SUCCESS || die (-2, "Could not connect", rc);
    fprintf (stdout, "OK\n");

    fprintf (stdout, "Sending connect message.");
    {
      stomp_frame frame;
      frame.command = "CONNECT";
      frame.headers = apr_hash_make (pool);
      apr_hash_set (frame.headers, "login", APR_HASH_KEY_STRING,
		    "mcollective");
      apr_hash_set (frame.headers, "passcode", APR_HASH_KEY_STRING,
		    "marionette");
      frame.body = NULL;
      frame.body_length = -1;
      rc = stomp_write (connection, &frame, pool);
      rc == APR_SUCCESS || die (-2, "Could not send frame", rc);
    }

  fprintf(stdout, "Sending Subscribe.");                                                                           
  {                                                                                                                
     stomp_frame frame;                                                                                            
     frame.command = "SUB";                                                                                        
     frame.headers = apr_hash_make(pool);                                                                          
     apr_hash_set(frame.headers, "destination", APR_HASH_KEY_STRING, "/topic/mcollective.discovery.command");      
         frame.body_length = -1;                                                                                   
     frame.body = NULL;                                                                                            
     rc = stomp_write(connection, &frame, pool);                                                                   
     rc==APR_SUCCESS || die(-2, "Could not send frame", rc);                                                       
  }                                                                                                                
  fprintf(stdout, "OK\n");                                                                                         

    DiscoveryAgent discoveryAgent(connection,pool);
    while (1)
      {
	fprintf (stdout, "Reading Response.");
	{
	  stomp_frame *frame;
	  rc = stomp_read (connection, &frame, pool);
	  rc == APR_SUCCESS || die (-2, "Could not read frame", rc);
	    fprintf (stdout, "OK\n");
	    discoveryAgent.handle(frame);
	}
      }

  }
}
