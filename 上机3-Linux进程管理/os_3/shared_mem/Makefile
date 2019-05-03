all:sender receiver
sender:sender.c init.c
	gcc sender.c init.c -pthread -o sender
receiver:receiver.c init.c
	gcc receiver.c init.c -pthread -o receiver