relauncher: 
	gcc relauncher.c server.c messages.pb-c.c storage.c login_data.c log.c -l protobuf-c -o relauncher -pthread -lrt -lm

client:
	gcc client_teste.c messages.pb-c.c -l protobuf-c -o client -pthread

clean:
	rm -f *.o *.~ relauncher client *.gch
