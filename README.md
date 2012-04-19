# Description

This is trial code for getting an mcollective agent running in C++.

# Why
The reason is to make this code was to reduce the memory footprint of the mcollective process.
By default this is running a ruby process, which takes quite some memory and has know memory leaks.

We run a lot of containers on the same box, and  X x (ruby memory-footprint) = LOTS of memory waisted

By creating a cpp we hope to reduce the memory footprint and leverage shared libraries reuse.

# Setting up Mcollective

The mcollective setup for testing consists of one box that runs both mcollective agent and server.
As the base OS we used one of the Amazon AMI (Centos compatible).

I pretty much followed <http://docs.puppetlabs.com/mcollective/reference/basic/gettingstarted.html>

## ActiveMQ

First we need java, version used :

    [root@mc-pdb ~]# java -version
      java version "1.6.0_22"
      OpenJDK Runtime Environment (IcedTea6 1.10.6) (amazon-52.1.10.6.44.amzn1-x86_64)
    OpenJDK 64-Bit Server VM (build 20.0-b11, mixed mode)

  - Downloaded apache-activemq-5.4.3 and extracted it
  - Chang the conf/activemq.xml with the file in the [getting started guide](http://docs.puppetlabs.com/mcollective/reference/basic/gettingstarted.html)
  - Changed bin/activemq to bind to ipv4

    ACTIVEMQ_OPTS_MEMORY="-Xms256M -Xmx256M  -Djava.net.preferIPv4Stack=true"


## Stomp Gem

  The mcollective rpm require the stomp gem to be packaged. A simple gem install stomp is not enough.
The rubygem-stomp is available in the Extras Packages for Enterprise Linux 6 for x86_64 (via EPEL)

  Note: On the Amazon AMI epel is installed but not enabled.

    # yum install -y rubygem-stomp

##  Mcollective installation

Puppetlabs provides a yum repo via http://yum.puppetlabs.com/ with rpms for . I downloaded them manually

    # rpm -ihV mcollective-1.2.1-1.el6.noarch.rpm  mcollective-client-1.2.1-1.el6.noarch.rpm  mcollective-common-1.2.1-1.el6.noarch.rpm

The config files are in /etc/mcollective/client.cfg and /etc/mcollective/server.cfg

To make them match the sample config file. (in both files)

    plugin.stomp.host = localhost
    plugin.stomp.port = 6163
    plugin.stomp.user = mcollective
    plugin.stomp.password = marionette

  Now you can start mcollective server

    # /etc/init.d/mcollective start

  Test it with

    # mc-ping

## Setting up mc-puppetd

The puppet-agent information can be found at <http://projects.puppetlabs.com/projects/mcollective-plugins/wiki/AgentPuppetd>

    # git clone git://github.com/puppetlabs/mcollective-plugins.git

    # cp puppetd/... -> /usr/libexec/mcollective/mcollective

    # agent ->agent puppetd.rb and puppetd.ddl
    # application -> application directory puppetd

    # mco puppetd  -v runonce

# Compiling the Agent

## Setup the required compiler tools

    # yum groupinstall "Development Tools"


## LibAQMP approach
See readme on libamqp

Note: the libamqp-cpp takes about 54 Meg to initalize :(

## Libstomp approach
Instead of using a generic amqp-cpp library and then implementing stomp on top.
Libyaml provides a stomp only interface, making it much more lighter

Sample code can be found at:
<http://svn.stomp.codehaus.org/browse/stomp/trunk/c/src/main.c?r=31>

To build it I found 
<http://bivald.com/lessons-learned/2009/11/building_libstomp_client_on_li.html>

## Required libraries
To make it work we require some libraries that are standard not (yet) available as rpm.
We install these libraries in /opt as prefix to avoid

- ActiveMQ-cpp : <http://activemq.apache.org/cms/index.html>
- APR : <http://apr.apache.org/>
- APR-util : <http://apr.apache.org/>
- Yaml-cpp : <http://code.google.com/p/yaml-cpp/>

libapr-1 and libapr-util are already available on centos but ActiveMQ-cpp needs more recent versions

<http://dev.ariel-networks.com/apr/apr-tutorial/html/apr-tutorial-13.html>
Good for debugging - <https://github.com/morellon/stomp/blob/master/bin/stompcat>


### APR

		yum install libapr-1 libapr1-dev

### Yaml-cpp
Yaml-cpp : <http://code.google.com/p/yaml-cpp/>

Requires cmake

    # yum install cmake

To have it use /opt as prefix - <http://stackoverflow.com/questions/6003374/what-is-cmake-equivalent-of-configure-prefix-dir-make-all-install>

    # wget http://yaml-cpp.googlecode.com/files/yaml-cpp-0.3.0.tar.gz
    # cd yaml-cpp-0.3.0
    # mkdir build
    # cd build
    # cmake -DCMAKE_INSTALL_PREFIX:PATH=/opt ..

## Agent coding information
### Helpful resources

  - Mcollective in other languages: <http://www.devco.net/archives/2010/08/07/mcollective_and_other_languages.php>
  - Message flow in Mcollective: <http://docs.puppetlabs.com/mcollective/screencasts.html#message_flow>
  - Message format: <http://docs.puppetlabs.com/mcollective/reference/basic/messageformat.html>
  - ActiveMQ security - mcollective <http://docs.puppetlabs.com/mcollective/reference/integration/activemq_security.html>

  - AMQP examples: <http://activemq.apache.org/cms/example.html>
  - Stomp (alternative library) <http://docs.codehaus.org/display/STOMP/Getting+Started+with+libstomp>
  - AMQP-CPP API: <http://activemq.apache.org/cms/api_docs/cms-1.2/>

  - Emit yaml - <http://code.google.com/p/yaml-cpp/wiki/HowToEmitYAML>
  - Parse yaml - <http://code.google.com/p/yaml-cpp/wiki/HowToParseADocument>

  - Generic c++/shared libs - <http://stackoverflow.com/questions/496664/c-dynamic-shared-library-on-linux/497158#497158>

### Yaml serializer
  By default the PSK security uses Ruby Marshaling to transfer information over stomp.
  The ssl security provider of mcollective allows for yaml by setting plugin.ssl_serialize = yaml

  I found good information in :

  - Ruby marshal code in php : <https://github.com/jaytaph/MCollectivePHP/blob/master/marshal.php>
  - Mcollective agent in php: <http://www.adayinthelifeof.nl/2011/10/01/creating-mcollective-agents-in-php-the-hard-way/ >
  - Mcollective stomp in php:  <https://github.com/jaytaph/MCollectivePHP/blob/master/stomp.php>

I adapted the /usr/libexec/mcollective/mcollective/plugins/agent/security/psk.rb (see psk.rb)

  Changing this requires a restart of the mcollective server

### What I learned
- An agent needs to respond to the discovery protocol to be used. Otherwise no requests are send. This might change in the new 1.3 protocol (dixit @ripienaar)
- The message is transferred as a binary message (even when using yaml)
- Both the message and the body are serialized separate. This requires you to unserialize first the message and then the body
- If mcollective gets a bad psk, it silently drops it
- You can easily make an mcollective agent that lies, not sure how to tackle that one. Like it replies ok,etc.. but in reality a hacker has control of your box.
- You get the callerid: uid=0  on the client
- It seems to ignore the msgtime added in the message
- there seems to be duplication with the describing the /topic/destination within the message itself too

### On to the real code

UGGLY!!

All code is serious copy and paste code and in no way polished, provided for exploration only.
It even doesn't run correctly. In general it does:

- register itself to the stomp/amqp bus
- listens for messages (mcollective.discovery.command)
- when it receives one, it will unpack (yaml)
- connect to the (mcollective.discovery.reply)

Files description:

- client.cfg and server.cfg used
- psk.rb adapted (mcollective 1.2)

After compiling run it with to make it work with the libs in /opt (I'm working on static compile too)

    # LD_LIBRARY_PATH=/opt ./mcpp

TODO: detail this more!!

### Looking at the memory usage
We don't want the kernel to lie to us, these are good ways to look at the actual memory usage.

    # cat /proc/pid/smaps
    # pmap -d pid

- The normal mcollective ruby needs about 19 Megs to run
- The libmamp-cpp needs about 54!Megs to run
- The libstomp version requires about 840K to run

