#------------------------------------------------------------------------------
# --soname vs macosx
# http://www.corpit.ru/pipermail/udns/2011q1/000124.html

SOURCE=mc-discovery.cpp
#SOURCE=x1.cpp
PROGRAM=mcpp
MYINCLUDES=-I/opt/include/activemq-cpp-3.4.1

MYLIBRARIES=
#MYLIBRARIES=-L/opt/lib -lactivemq-cpp -lyaml-cpp -lapr-1 -laprutil-1 -lpthread
MYLIBRARIES=-L/opt/lib -lactivemq-cpp -lyaml-cpp -lapr-1 -laprutil-1 -lcrypto  -lssl  -lpthread
#MYLIBRARIES=-L/opt/lib -Wl,-Bstatic -lssl -lcrypto -lapr-1 -laprutil-1 -lactivemq-cpp -lyaml-cpp -Wl,-Bdynamic  -lpthread -lssl -lcrypto
#MYLIBRARIES=-L/usr/lib64 -Wl,-Bstatic -lssl -lcrypto -L/opt/lib -Wl,-Bdynamic -lapr-1 -laprutil-1 -lactivemq-cpp -lyaml-cpp -lpthread
CXX=g++

#------------------------------------------------------------------------------

main: libmcollective.so main.o
	$(CXX) -Os -o $(PROGRAM) main.o -L. -lmcollective -lapr-1 -lyaml-cpp -lcrypto -lssl

main.o: main.cpp
	$(CXX) -Os -c main.cpp -lmcollective -lstomp

listener.o: listener.cpp
	$(CXX) -Os -fPIC -c listener.cpp -o listener.o  -I/usr/include/apr-1 -I.

stomp.o: stomp.c
	gcc -Os -fPIC -c stomp.c -I.  -I/usr/include/apr-1 -lapr-1

agent/discovery.o: agent/discovery.cpp
	$(CXX) -Os -fPIC -c agent/discovery.cpp -o agent/discovery.o  -I/usr/include/apr-1 -I. -I/opt/include/yaml-cpp

libmcollective.so: listener.o agent/discovery.o stomp.o
	$(CXX) -Os -shared -Wl -o libmcollective.so listener.o agent/discovery.o stomp.o

clean:
	rm *.o *.so $(PROGRAM)
