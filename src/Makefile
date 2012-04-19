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
	$(CXX) -o $(PROGRAM) main.o -L. -lmcollective

libmcollective.so: mcollective_listener.cpp
	$(CXX) -fPIC -c mcollective_listener.cpp -o mcollective.o
	$(CXX) -shared -Wl -o libmcollective.so mcollective.o

clean:
	rm *.o *.so $(PROGRAM)
