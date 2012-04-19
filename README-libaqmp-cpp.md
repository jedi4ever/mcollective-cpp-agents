## Required libraries
To make it work we require some libraries that are standard not (yet) available as rpm.
We install these libraries in /opt as prefix to avoid

- ActiveMQ-cpp : <http://activemq.apache.org/cms/index.html>
- APR : <http://apr.apache.org/>
- APR-util : <http://apr.apache.org/>
- Yaml-cpp : <http://code.google.com/p/yaml-cpp/>

libapr-1 and libapr-util are already available on centos but ActiveMQ-cpp needs more recent versions

### APR
APR : <http://apr.apache.org/>

    # wget http://apache.cu.be//apr/apr-1.4.6.tar.gz
    # tar -xzvf apr-1.4.6.tar.gz
    # cd apr-1.4.6
    # ./configure --prefix=/opt
    # make install

### APR-util
APR-util : <http://apr.apache.org/>

    # wget http://apache.cu.be//apr/apr-util-1.4.1.tar.gz
    # tar -xzvf apr-1.4.1.tar.gz
    # cd apr-1.4.1
    # ./configure --prefix=/opt --with-apr=/opt
    # make install

### Activemq-cpp
ActiveMQ-cpp : <http://activemq.apache.org/cms/index.html>

This requires apr and apr-util: <http://stackoverflow.com/questions/7957837/activemq-c-client-install-says-apr-is-not-installed-why>

Requires openssl-devel (but doesn't seem to work)

    # yum install openssl-devel

    # wget http://apache.megamobile.be/activemq/activemq-cpp/source/activemq-cpp-library-3.4.1-src.tar.gz
    # tar -xzvf activemq-cpp-library-3.4.1-src.tar.gz
    # cd activemq-cpp-library-3.4.1
    # ./configure --prefix=/opt --with-apr=/opt/ --with-apr-util=/opt --disable-ssl
    # make install

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

