#include "agent/discovery.h"
#include "listener.h"
#include "stomp.h"
#include <pthread.h>

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

				struct thread_data{
								apr_pool_t *pool;                 
								stomp_connection *connection;
								int  thread_id;
				};

				void *DiscoveryThread(void *threadarg) {
								struct thread_data *my_data;
								my_data = (struct thread_data *) threadarg;
								DiscoveryAgent agent(my_data->connection,my_data->pool);
								printf("starting");
								agent.start();
				}

				Listener::Listener ()
				{
				}

				void Listener::StartThreads ()
				{
								apr_status_t rc;
								rc = apr_initialize ();
								rc == APR_SUCCESS || die (-2, "Could not initialize", rc);
								atexit (terminate);

								pthread_t threads[2];
								size_t stacksize;
								pthread_attr_t attr;
								pthread_attr_init(&attr);

								//https://computing.llnl.gov/tutorials/pthreads/
								struct thread_data thread_data_array[2];

								thread_data_array[0].thread_id = 0;	
								rc = pthread_create(&threads[0], NULL, DiscoveryThread, (void *) &thread_data_array[0] );
								thread_data_array[1].thread_id = 1;	
								rc = pthread_create(&threads[1], NULL, DiscoveryThread, (void *) &thread_data_array[1] );
							

				}

}
