//Constructor passes the session

// // Create the destination (Topic or Queue)
//  destination = session->createTopic ("mcollective.discovery.command");


//  // Create a MessageConsumer from the Session to the Topic or Queue
//  consumer = session->createConsumer (destination);

// Set listener (a class that has onMessage(const Message * message) throw()
// consumer->setMessageListener (this);
//
//
//


// Called from the consumer since this class is a registered MessageListener
//  virtual void
//     onMessage (const Message * message)
//        throw ()

// extract message to yaml and pass it to handle

   // const BytesMessage *
   //  bytesMessage = dynamic_cast < const
   //  BytesMessage * >(message);

// if (bytesMessage != NULL)
// {
//   //std::cout << bytesMessage->getBodyBytes();
//   //bytesMessage->reset();
//
//   size_t i = bytesMessage->getBodyLength ();
//   printf ("%lu", i);
//   ofstream ofs ("message.yaml", ofstream::out);
//   for (int x = 1; x <= i; x++)
//   {
//     ofs << bytesMessage->readByte ();
//   }
//   ofs.flush ();
//   ofs.close ();
//
//   try
//   {
//     std::ifstream fin ("message.yaml");
//     //std::stringstream fin(std::string(bytesMessage->getBodyBytes()));
//     YAML::Parser parser (fin);
//     YAML::Node doc;
//     // We assume only the first doc, need to check with doc.size
//     parser.GetNextDocument (doc);


/////////////////////// String to YAML
// std::stringstream bodystream (body);
// YAML::Parser bodyparser (bodystream);
// YAML::Node bodydoc;
// std::string action;
// bodyparser.GetNextDocument (bodydoc);
// bodydoc >> action;
// std::cout << action;

///////////////  Construct body
// // Construct YAML body
// YAML::Emitter reply_message_body_yaml;
// reply_message_body_yaml << "pong";

///////////// MD5 sign body
//  std::string reply_message_body = reply_message_body_yaml.c_str();
//  std::cout << reply_message_body << std::endl;
//  // Append PSK to it
//  std::string psk = "unset";
//  std::string body_psk = reply_message_body;
//  body_psk.append(psk);
//  std::stringstream md5sumstream;

// // MD5 -  https://gist.github.com/2389719
// // but needs a real string
// // http://social.msdn.microsoft.com/Forums/en/Vsexpressvc/thread/e1774395-ba99-4fe6-98eb-2224a67984b9
// unsigned char md5_result[MD5_DIGEST_LENGTH];
// const unsigned char * constStr = reinterpret_cast<const unsigned char *> (body_psk.c_str());
// MD5(constStr, body_psk.length() , md5_result);
// for (i=0; i < MD5_DIGEST_LENGTH; i++)
// {
//   printf("%02x",  md5_result[i]);
//   char digit[2];
//   sprintf(digit,"%02x",  md5_result[i]);
//   md5sumstream << digit;
// }
// printf("\\n");
//


// Construct answer

// YAML::Emitter reply_message_yaml;
//
// reply_message_yaml << YAML::BeginMap;
// reply_message_yaml << YAML::Key << ":msgtime";
// reply_message_yaml << YAML::Value << 1010101;
// reply_message_yaml << YAML::Key << ":requestid";
// reply_message_yaml << YAML::Value << requestid;
// reply_message_yaml << YAML::Key << ":body";
// reply_message_yaml << YAML::Value << reply_message_body;
// reply_message_yaml << YAML::Key << ":senderid";
// reply_message_yaml << YAML::Value << "mcpp";
// reply_message_yaml << YAML::Key << ":senderagent";
// reply_message_yaml << YAML::Value << "discovery";
// reply_message_yaml << YAML::Key << ":msgtarget";
// reply_message_yaml << YAML::Value << "/topic/mcollective.discovery.reply";
//
// reply_message_yaml << YAML::Key << ":hash";
// reply_message_yaml << YAML::Value << hash;
// reply_message_yaml << YAML::EndMap;
//
//
// // Put it in a string
// std::string reply_message = reply_message_yaml.c_str();
// std::cout << reply_message << std::endl;

  //   // Create the destination (Topic or Queue)
  //   destination = session->createTopic( "mcollective.discovery.reply" );

  //   // Create a MessageProducer from the Session to the Topic or Queue
  //   producer = session->createProducer( destination );
  //   producer->setDeliveryMode( DeliveryMode::NON_PERSISTENT );

  //   // Create a messages
  //   BytesMessage* reply = session->createBytesMessage();

  //   reply->writeString(reply_message.c_str());
  //   producer->send( reply );
  //   printf("reply send \\n");

  //   delete reply;

  // }catch ( CMSException& e ) {
  //   e.printStackTrace();
  // }
