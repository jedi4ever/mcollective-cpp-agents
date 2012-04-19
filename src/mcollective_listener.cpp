#include "agent/base.h"
#include "mcollective_listener.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

using namespace
std;

McollectiveListener::McollectiveListener() {
}

void McollectiveListener::DoSomething() {

  // Initialize the library
  //activemq::library::ActiveMQCPP::initializeLibrary ();

  //Set the connection string
  std::string brokerURI = "tcp://127.0.0.1:6163" "?wireFormat=stomp";

  std::cout << "=====================================================\n";
  std::cout << "Starting Listener:" << std::endl;
  std::cout << "-----------------------------------------------------\n";

  // Create a ConnectionFactory
  // auto_ptr < ConnectionFactory >
  //   connectionFactory (ConnectionFactory::createCMSConnectionFactory
  //         (brokerURI));
  //

  // Create a Connection
  // connection =
  //   connectionFactory->createConnection ("mcollective", "marionette");
  //   connection->start ();
  //   connection->setExceptionListener (this);

  // Create a Session
  //session = connection->createSession (Session::AUTO_ACKNOWLEDGE);

  //DiscoveryAgent discoveryAgent(&session);
  //PuppetAgent puppetAgent(&session);

}
