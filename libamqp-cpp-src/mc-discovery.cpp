/*w:vim: ts=2 sw=2 sts=2: */

/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// START SNIPPET: demo

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/md5.h>

#include <activemq/library/ActiveMQCPP.h>

#include "yaml-cpp/yaml.h"

#include <decaf/lang/Thread.h>
#include <decaf/lang/Runnable.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <decaf/lang/Integer.h>
#include <decaf/util/Date.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/util/Config.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>
#include <cms/BytesMessage.h>
#include <cms/MapMessage.h>
#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>
#include <iostream>
#include <memory>

#include <fstream>

using namespace
activemq::core;
using
namespace
decaf::util::concurrent;
using
namespace
decaf::util;
using
namespace
decaf::lang;
using
namespace
cms;
using
namespace
std;

class HelloWorldConsumer:
  public ExceptionListener, public MessageListener, public Runnable
{

  private:

    CountDownLatch latch;
    CountDownLatch doneLatch;
    Connection *
      connection;
    Session *
      session;
    Destination *
      destination;
    MessageConsumer *
      consumer;
    long
      waitMillis;
    bool useTopic;
    bool sessionTransacted;
    std::string brokerURI;

  public:

    HelloWorldConsumer (const std::string & brokerURI, long numMessages, bool useTopic = false, bool sessionTransacted = false, long waitMillis = 3000000):latch (1),
    doneLatch
             (numMessages)
  {
    this->connection = NULL;
    this->session = NULL;
    this->destination = NULL;
    this->consumer = NULL;
    this->waitMillis = waitMillis;
    this->useTopic = useTopic;
    this->sessionTransacted = sessionTransacted;
    this->brokerURI = brokerURI;
  }
    virtual ~ HelloWorldConsumer ()throw ()
    {
      cleanup ();
    }

    void
      waitUntilReady ()
      {
        latch.await ();
      }

    virtual void
      run ()
      {

        try
        {

          // Create a ConnectionFactory
          auto_ptr < ConnectionFactory >
            connectionFactory (ConnectionFactory::createCMSConnectionFactory
                (brokerURI));

          // Create a Connection
          connection =
            connectionFactory->createConnection ("mcollective", "marionette");
          connection->start ();
          connection->setExceptionListener (this);

          // Create a Session
          if (this->sessionTransacted == true)
          {
            session = connection->createSession (Session::SESSION_TRANSACTED);
          }
          else
          {
            session = connection->createSession (Session::AUTO_ACKNOWLEDGE);
          }

          // Create the destination (Topic or Queue)
          if (useTopic)
          {
            destination = session->createTopic ("mcollective.discovery.command");
          }
          else
          {
            destination = session->createQueue ("TEST.FOO");
          }

          // Create a MessageConsumer from the Session to the Topic or Queue
          consumer = session->createConsumer (destination);

          consumer->setMessageListener (this);

          std::cout.flush ();
          std::cerr.flush ();

          // Indicate we are ready for messages.
          latch.countDown ();

          // Wait while asynchronous messages come in.
          doneLatch.await (waitMillis);

        }
        catch (CMSException & e)
        {

          // Indicate we are ready for messages.
          latch.countDown ();

          e.printStackTrace ();
        }
      }

    // Called from the consumer since this class is a registered MessageListener.
    virtual void
      onMessage (const Message * message)
      throw ()
      {

        static int
          count = 0;

        try
        {
          count++;
          const TextMessage *
            textMessage = dynamic_cast < const
            TextMessage * >(message);

          const BytesMessage *
            bytesMessage = dynamic_cast < const
            BytesMessage * >(message);

          string text = "";

          if (bytesMessage != NULL)
          {
            //std::cout << bytesMessage->getBodyBytes();
            //bytesMessage->reset();

            size_t i = bytesMessage->getBodyLength ();
            printf ("%lu", i);
            ofstream ofs ("message.yaml", ofstream::out);
            for (int x = 1; x <= i; x++)
            {
              ofs << bytesMessage->readByte ();
            }
            ofs.flush ();
            ofs.close ();

            try
            {
              std::ifstream fin ("message.yaml");
              //std::stringstream fin(std::string(bytesMessage->getBodyBytes()));
              YAML::Parser parser (fin);
              YAML::Node doc;
              // We assume only the first doc, need to check with doc.size
              parser.GetNextDocument (doc);

              /*
Key: :agent
Key: :body
Key: :callerid
Key: :collectiv
Key: :filter
Key: :hash
Key: :msgtarget
Key: :msgtime
Key: :requestid
Key: :senderid
*/
              for (YAML::Iterator it = doc.begin (); it != doc.end (); ++it)
              {
                std::string key, value;
                it.first () >> key;
                std::cout << "Key: " << key << std::endl;
              }

              std::string requestid;
              std::string senderid;
              std::string msgtarget;
              doc[":msgtarget"] >> msgtarget;
              doc[":requestid"] >> requestid;
              doc[":senderid"] >> senderid;
              std::cout << msgtarget << std::endl << requestid << std::
                endl << senderid << std::endl;

              // Body seems to be multiline string of yaml
              // Parsing strings http://stackoverflow.com/questions/2813030/yaml-cpp-parsing-strings
              std::string body;
              doc[":body"] >> body;

              std::stringstream bodystream (body);
              YAML::Parser bodyparser (bodystream);
              YAML::Node bodydoc;
              std::string action;
              bodyparser.GetNextDocument (bodydoc);
              bodydoc >> action;
              std::cout << action;


              // Construct YAML body
              YAML::Emitter reply_message_body_yaml;
              reply_message_body_yaml << "pong";

              // Put it in a string
              std::string reply_message_body = reply_message_body_yaml.c_str();
              std::cout << reply_message_body << std::endl;
              // Append PSK to it
              std::string psk = "unset";
              std::string body_psk = reply_message_body;
              body_psk.append(psk);

              std::stringstream md5sumstream;
              // MD5 -  https://gist.github.com/2389719
              // but needs a real string
              // http://social.msdn.microsoft.com/Forums/en/Vsexpressvc/thread/e1774395-ba99-4fe6-98eb-2224a67984b9
              unsigned char md5_result[MD5_DIGEST_LENGTH];
              const unsigned char * constStr = reinterpret_cast<const unsigned char *> (body_psk.c_str());
              MD5(constStr, body_psk.length() , md5_result);
              for (i=0; i < MD5_DIGEST_LENGTH; i++)
              {
                printf("%02x",  md5_result[i]);
                char digit[2];
                sprintf(digit,"%02x",  md5_result[i]);
                md5sumstream << digit;
              }
              printf("\n");

              std::cout << "md5 stream:" << md5sumstream.str() << std::endl;
              std::string hash = md5sumstream.str();
              std::cout << "hash:" << hash << std::endl;

              YAML::Emitter reply_message_yaml;

              reply_message_yaml << YAML::BeginMap;
              reply_message_yaml << YAML::Key << ":msgtime";
              reply_message_yaml << YAML::Value << 1010101;
              reply_message_yaml << YAML::Key << ":requestid";
              reply_message_yaml << YAML::Value << requestid;
              reply_message_yaml << YAML::Key << ":body";
              reply_message_yaml << YAML::Value << reply_message_body;
              reply_message_yaml << YAML::Key << ":senderid";
              reply_message_yaml << YAML::Value << "mcpp";
              reply_message_yaml << YAML::Key << ":senderagent";
              reply_message_yaml << YAML::Value << "discovery";
              reply_message_yaml << YAML::Key << ":msgtarget";
              reply_message_yaml << YAML::Value << "/topic/mcollective.discovery.reply";

              reply_message_yaml << YAML::Key << ":hash";
              reply_message_yaml << YAML::Value << hash;
              reply_message_yaml << YAML::EndMap;


              // Put it in a string
              std::string reply_message = reply_message_yaml.c_str();
              std::cout << reply_message << std::endl;


              // Get ready to send
              try {

                Connection* connection;
                Session* session;
                Destination* destination;
                MessageProducer* producer;
                // Create a ConnectionFactory
                std::string brokerURI = "tcp://127.0.0.1:6163" "?wireFormat=stomp";

                auto_ptr<ConnectionFactory> connectionFactory(
                    ConnectionFactory::createCMSConnectionFactory( brokerURI ) );

                // Create a Connection
                connection = connectionFactory->createConnection ("mcollective", "marionette");
                connection->start();

                // Create a Session
                session = connection->createSession( Session::AUTO_ACKNOWLEDGE );
                // Create the destination (Topic or Queue)
                destination = session->createTopic( "mcollective.discovery.reply" );

                // Create a MessageProducer from the Session to the Topic or Queue
                producer = session->createProducer( destination );
                producer->setDeliveryMode( DeliveryMode::NON_PERSISTENT );

                // Create a messages
                BytesMessage* reply = session->createBytesMessage();

                reply->writeString(reply_message.c_str());
                producer->send( reply );
                printf("reply send \n");

                delete reply;

              }catch ( CMSException& e ) {
                e.printStackTrace();
              }


            }
            catch (YAML::ParserException & e)
            {
              std::cout << e.what () << "\n";
            }
          }

          if (textMessage != NULL)
          {
            text = textMessage->getText ();
          }
          else
          {
            text = "NOT A TEXTMESSAGE!";
            //std::cout << message->getCMSType();
            std::vector < string > v = message->getPropertyNames ();
            for (std::vector < string >::iterator it = v.begin ();
                it != v.end (); ++it)
            {
              std::cout << *it << std::endl;
            }
          }
          printf ("Message #%d Received: %s\n", count, text.c_str ());




        }
        catch (CMSException & e)
        {
          e.printStackTrace ();
        }

        // Commit all messages.
        if (this->sessionTransacted)
        {
          session->commit ();
        }

        // No matter what, tag the count down latch until done.
        doneLatch.countDown ();
      }

    // If something bad happens you see it here as this class is also been
    // registered as an ExceptionListener with the connection.
    virtual void
      onException (const CMSException & ex AMQCPP_UNUSED)
      {
        printf ("CMS Exception occurred.  Shutting down client.\n");
        exit (1);
      }

  private:

    void
      cleanup ()
      {

        //*************************************************
        // Always close destination, consumers and producers before
        // you destroy their sessions and connection.
        //*************************************************

        // Destroy resources.
        try
        {
          if (destination != NULL)
            delete destination;
        }
        catch (CMSException & e)
        {
          e.printStackTrace ();
        }
        destination = NULL;

        try
        {
          if (consumer != NULL)
            delete consumer;
        }
        catch (CMSException & e)
        {
          e.printStackTrace ();
        }
        consumer = NULL;

        // Close open resources.
        try
        {
          if (session != NULL)
            session->close ();
          if (connection != NULL)
            connection->close ();
        }
        catch (CMSException & e)
        {
          e.printStackTrace ();
        }

        // Now Destroy them
        try
        {
          if (session != NULL)
            delete session;
        }
        catch (CMSException & e)
        {
          e.printStackTrace ();
        }
        session = NULL;

        try
        {
          if (connection != NULL)
            delete connection;
        }
        catch (CMSException & e)
        {
          e.printStackTrace ();
        }
        connection = NULL;
      }
};

  int
main (int argc AMQCPP_UNUSED, char *argv[]AMQCPP_UNUSED)
{


  activemq::library::ActiveMQCPP::initializeLibrary ();

  std::cout << "=====================================================\n";
  std::cout << "Starting the example:" << std::endl;
  std::cout << "-----------------------------------------------------\n";

  // Set the URI to point to the IP Address of your broker.
  // add any optional params to the url to enable things like
  // tightMarshalling or tcp logging etc.  See the CMS web site for
  // a full list of configuration options.
  //
  //  http://activemq.apache.org/cms/
  //
  // Wire Format Options:
  // =====================
  // Use either stomp or openwire, the default ports are different for each
  //
  // Examples:
  //    tcp://127.0.0.1:61616                      default to openwire
  //    tcp://127.0.0.1:61616?wireFormat=openwire  same as above
  //    tcp://127.0.0.1:61613?wireFormat=stomp     use stomp instead
  //
  std::string brokerURI = "tcp://127.0.0.1:6163" "?wireFormat=stomp"
    //        "&connection.alwaysSyncSend=true"
    //        "&connection.useAsyncSend=true"
    //        "&transport.commandTracingEnabled=true"
    //        "&transport.tcpTracingEnabled=true"
    //        "&wireFormat.tightEncodingEnabled=true"
    ;

  //============================================================
  // set to true to use topics instead of queues
  // Note in the code above that this causes createTopic or
  // createQueue to be used in both consumer an producer.
  //============================================================
  bool useTopics = true;
  bool sessionTransacted = false;
  int
    numMessages = 2000;

  //long long startTime = Date::getCurrentTimeMilliseconds();

  HelloWorldConsumer
    consumer (brokerURI, numMessages, useTopics, sessionTransacted);

  // Start the consumer thread.
  Thread consumerThread (&consumer);
  consumerThread.start ();

  // Wait for the consumer to indicate that its ready to go.
  consumer.waitUntilReady ();

  // Wait for the threads to complete.
  consumerThread.join ();

  //long long endTime = Date::getCurrentTimeMilliseconds();
  //double totalTime = (endTime - startTime) / 1000.0;

  //std::cout << "Time to completion = " << totalTime << " seconds." << std::endl;
  std::cout << "-----------------------------------------------------\n";
  std::cout << "Finished with the example." << std::endl;
  std::cout << "=====================================================\n";
}

// END SNIPPET: demo
//
