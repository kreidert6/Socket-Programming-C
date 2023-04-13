/**
 * client.c
 *
 * @author Tyler Kreider
 *
 * USD COMP 375: Computer Networks
 * Project 1
 *
 * This program will prompt the user to choose a statistic (air temp, wind
 * speed, humidity) to get from the USD campus. It will contact the server
 * that collects this data and will grab this information and present it to
 * user. 
 */

#define _XOPEN_SOURCE 600

#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

long prompt();
int connectToHost(char *hostname, char *port);
//void mainLoop(int server_fd);
void mainLoop();
void getInfo(int server_fd, char *msg, char* port, char* buff);


int main() {
	mainLoop();
	return 0;
}

/**
 * Loop to keep asking user what they want to do and calling the appropriate
 * function to handle the selection.
 *
 * @param server_fd Socket file descriptor for communicating with the server
 *
*/
//void mainLoop(int server_fd) {
void mainLoop(){
	char port[6];
	port[6] = '\0';
	char buff[1500];
	int server_fd;	
	int server_fd1;
	int server_fd2;
	int server_fd3;
	int server_fd4;
    int server_fd5;
	printf("\nWELCOME TO THE COMP375 SENSOR NETWORK\n\n");
	while (1) {
		long selection = prompt();
		time_t curtime;

	    time(&curtime);

		switch (selection) {
			case 1:
				//initial connection to server
				server_fd = connectToHost("main.esmarttech.com", "47789");
				
				//call getInfo to get information			
				getInfo(server_fd, "AUTH password123\n", port, buff);
				
				//connect to new destination
				server_fd1 = connectToHost("sensor.esmarttech.com", port);				
				//call getInfo to authenticate
				getInfo(server_fd1, "AUTH sensorpass321\n", port, buff);
				//call getInfo to request specific info 
				getInfo(server_fd1, "AIR TEMPERATURE\n", port, buff);		
				
				char temp[3];
				strncpy(temp, buff+11, 2);
				temp[2] = '\0';

				printf("\nThe last AIR TEMPERATURE reading was %s F, taken at %s", temp, ctime(&curtime)); 				
				
				break;
			//when user wants to get humidity
			case 2: 
				server_fd2 = connectToHost("main.esmarttech.com", "47789");    
                getInfo(server_fd2, "AUTH password123\n", port, buff);
    
                server_fd3 = connectToHost("sensor.esmarttech.com", port);    
                getInfo(server_fd3, "AUTH sensorpass321\n", port, buff);
                getInfo(server_fd3, "RELATIVE HUMIDITY\n", port, buff);    
                    
                char humidity[3];
                strncpy(humidity, buff+11, 2); 
				humidity[2] = '\0';
                printf("\nThe last RELATIVE HUMIDITY reading was %s %%, taken at %s", humidity, ctime(&curtime));
				
				
			break;

			//when user wants to get wind speed 
			case 3: 
				server_fd4 = connectToHost("main.esmarttech.com", "47789");    
                getInfo(server_fd4, "AUTH password123\n", port, buff);
                //printf("Port: %s\n", port);
    
                server_fd5 = connectToHost("sensor.esmarttech.com", port);    
                getInfo(server_fd5, "AUTH sensorpass321\n", port, buff);
                getInfo(server_fd5, "WIND SPEED\n", port, buff);    
                    
                char speed[3];;
                strncpy(speed, buff+11, 2); 
				speed[2] = '\0';
                printf("\nThe last WIND SPEED reading was %sMPH, taken at %s", speed, ctime(&curtime));
				break;	
			case 4:
				printf("GOODBYE!\n"); 
				exit(1);

			
	
			default:
				fprintf(stderr, "ERROR: Invalid selection\n");
				break;
		}
	}

}

/*
 * Print command prompt to user and obtain user input.
 *
 * @return The user's desired selection, or -1 if invalid selection.
 */
long prompt() {
	// add printfs to print out the options
	
    printf("\nWhich sensor would you like to read:\n");    
	printf("\n\t(1) Air Temperature\n");
    printf("\t(2) Relative humidity\n");
    printf("\t(3) Wind speed\n");
    printf("\t(4) Quit Program\n\n"); 
	printf("Selection: ");
	char input[10];
	memset(input, 0, 10); // set all characters in input to '\0' (i.e. nul)
	char *read_str = fgets(input, 10, stdin);

	// Check if EOF or an error, exiting the program in both cases.
	if (read_str == NULL) {
		if (feof(stdin)) {
			exit(0);
		}
		else if (ferror(stdin)) {
			perror("fgets");
			exit(1);
		}
	}

	// get rid of newline, if there is one
	char *new_line = strchr(input, '\n');
	if (new_line != NULL) new_line[0] = '\0';

	// convert string to a long int
	char *end;
	long selection = strtol(input, &end, 10);

	if (end == input || *end != '\0') {
		selection = -1;
	}

	return selection;
}

/**
 * Socket implementation of connecting to a host at a specific port.
 *
 * @param hostname The name of the host to connect to (e.g. "foo.sandiego.edu")
 * @param port The port number to connect to
 *
 * @return File descriptor of new socket to use.
 */
int connectToHost(char *hostname, char *port) {
	// Step 1: fill in the address info in preparation for setting 
	//   up the socket
	int status;
	struct addrinfo hints;
	struct addrinfo *servinfo;  // will point to the results

	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_INET;       // Use IPv4
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

	// get ready to connect
	if ((status = getaddrinfo(hostname, port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		exit(1);
	}

	// Step 2: Make a call to socket
	int fd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	if (fd == -1) {
		perror("socket");
		exit(1);
	}

	// Step 3: connect!
	if (connect(fd, servinfo->ai_addr, servinfo->ai_addrlen) != 0) {
		perror("connect");
		exit(1);
	}

	freeaddrinfo(servinfo); // free's the memory allocated by getaddrinfo

	return fd;
}

/**
 * This function will request information from the server through the socket
 * created and receive the information and put it into a buffer.
 *
 *
 * @param server_fd Socket file descriptor for communicating with the server
 * @param msg Message provided to the server to clarify its request
 * @param port Pointer to port number so it will update correctly
 * @param buff Buffer that holds the information received from server
 *
 * @return none
*/
void getInfo(int server_fd, char *msg, char* port, char* buff){

	
	int msgLen, bytes_rec;
    
	


	msgLen = strlen(msg);
	
	//request info from server
	send(server_fd, msg, msgLen, 0);

	//receives info from server
	bytes_rec = recv(server_fd, buff, 1500, 0);
	
	//update the port number	
	strncpy(port, buff+30, 5);
	
	
			
	buff[bytes_rec]=0;
	
	
	

}
