/***
Keenan Johnstone 	- 11119412 	- kbj182

CMPT 332 	Assignment 4	December 7, 2015
***/

README for Assignment 4 in CMPT332

Notes: 	-I am the only one working on this assignment, my previous partner 
	and I had a falling out.

Building:
	To build all simply use:
		make
	You can build single files with:
		make server
		make clientSender
		make clientReceiver
	Clean with:
		make clean
		
Running:
	To run the server use:
		./server
		
		This runs the server and displays the the IP address and the SEND/RECV ports
		
	To run the sender:
		./sender IP/HOSTNAME PORT

	To run the receiver:
		./receiver IP/HOSTNAME PORT
		
Once running:
	server:
		No input required
	
	sender:
		Type the desired message and press enter to send
		
	receiver:
		No input required
		
Known Issues:
	You can force the receiver to connect to the sender port and it causes some unintended behavior.

	