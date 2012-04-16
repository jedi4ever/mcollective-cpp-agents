#------------------------------------------------------------------------------

SOURCE=mc-discovery.cpp
#SOURCE=x1.cpp
MYPROGRAM=mcpp
MYINCLUDES=-I/opt/include/activemq-cpp-3.4.1

MYLIBRARIES=
#MYLIBRARIES=-L/opt/lib -lactivemq-cpp -lyaml-cpp -lapr-1 -laprutil-1 -lpthread
MYLIBRARIES=-L/opt/lib -lactivemq-cpp -lyaml-cpp -lapr-1 -laprutil-1 -lcrypto  -lssl  -lpthread
#MYLIBRARIES=-L/opt/lib -Wl,-Bstatic -lssl -lcrypto -lapr-1 -laprutil-1 -lactivemq-cpp -lyaml-cpp -Wl,-Bdynamic  -lpthread -lssl -lcrypto
#MYLIBRARIES=-L/usr/lib64 -Wl,-Bstatic -lssl -lcrypto -L/opt/lib -Wl,-Bdynamic -lapr-1 -laprutil-1 -lactivemq-cpp -lyaml-cpp -lpthread
CC=g++

#------------------------------------------------------------------------------



all: $(MYPROGRAM)

$(MYPROGRAM): $(SOURCE)

	$(CC) -Os $(MYINCLUDES) $(SOURCE) -o$(MYPROGRAM)   $(MYLIBRARIES)

clean:

	rm -f $(MYPROGRAM)

