#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "utils.h"

/* Given the specified protocol argument (can be a port number turned into a
   string or an application-layer protocol name), find the list of server
   addrinfo instances provided by getaddrinfo(). For this assignment, only use TCP
   over IPv4. Loop through the list to create and bind a socket. If everything
   is successful, use listen() to convert the socket to a server socket and
   return the file descriptor. Return -1 if any error occurs.
 */
int
setup_server (const char *protocol)
{
  struct addrinfo *server_list = NULL;
  struct addrinfo hints;
  memset (&hints, 0, sizeof (hints));
  // TODO: Set the hints fields for a TCP/IPv4 socket to be used as a
  // server. The hints struct is used by getaddrinfo() to determine if the
  // current system can be configured as a server for the requested protocol.
  
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo (NULL, protocol, &hints, &server_list) != 0)
  {
  	printf("getaddrinfo Error");
    return -1;
	}
  // Traverse through the server list, trying to create and bind a socket,
  // similar to how a client would work.
  // Traverse through the server list, trying to create and bind a socket
	int socketfd = -1;
	struct addrinfo *server = NULL;

	for (server = server_list; server != NULL; server = server->ai_next)
	{
		  // Create the socket
		  if ((socketfd = socket(server->ai_family, server->ai_socktype, server->ai_protocol)) < 0)
		      continue;

		  // Set the socket options
		  int socket_option = 1;
		  struct timeval timeout = {10, 0}; // 10.0 seconds

		  setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&socket_option, sizeof(int));
		  setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, (const void *)&timeout, sizeof(timeout));

		  // Attempt to bind the socket
		  if (bind(socketfd, server->ai_addr, server->ai_addrlen) == 0)
		      break;

		  // If bind fails, close the socket and reset socketfd
		  shutdown(socketfd, SHUT_RDWR);
		  close(socketfd);
		  socketfd = -1;
	}

	
  // TODO: Free the server list and convert the socket to a server socket.
  // At this point, we either have a socket (socketfd > 0) or we don't
  // (socketfd == -1). Either way, we do not need the list of addrinfo
  // structs any more, so free them using freeaddrinfo(). If we do have
  // a socket, use listen() to convert it to a server socket.
  
  
	freeaddrinfo(server_list);
	if (socketfd != -1)
		listen(socketfd, 5);
		
  return socketfd;
}

/* Wait for an incoming connection to arrive. When it does, use inet_ntoa()
   to return the string version of the IP address (use address.sin_addr) for
   printing. Set the call-by-reference parameter connection by dereferencing
   the pointer and setting the value to contain the file descriptor for this
   connection. If accept() returns a negative value, shutdown and close the
   socket before returning NULL. NOTE: inet_ntoa() (in contrast to
   inet_ntop()) returns a pointer to a statically allocated buffer, so the
   return value should NOT be freed.
 */
char *
get_connection (int socketfd, int *connection)
{
  // For this assignment, only use IPv4 addresses:
  struct sockaddr_in address;
  memset (&address, 0, sizeof (address));
  socklen_t addresslen = sizeof (struct sockaddr_in);
	
	char received_message[addresslen+1];
	read (socketfd, received_message, addresslen);
	received_message[addresslen] = '\0';
  // TODO: Accept the connection request and return the specified values
  // described above.
  *connection = accept (socketfd, (struct sockaddr *)&address, &addresslen);
  if (*connection < 0)
  	{
  		close(socketfd);
  		return NULL;
  	}
  
  return inet_ntoa (address.sin_addr);
}

/* Build the HTTP response for the requested URI and HTTP version. Don't
   forget that all HTTP header lines must end with \r\n, and there must
   be an additional \r\n following a blank line to separate the header
   from the body. For FULL requirements, support both HTTP/1.0 and HTTP/1.1,
   including a "Connection: close\r\n" header for version 1.1. Also, for
   FULL, dynamically allocate a buffer for the contents and read the file
   into that space (returning it through the contents call-by-reference
   parameter).

   Example: For srv_root/index.html and HTTP version 1.0, the returned
   header should be (ignore spaces at the beginning of the lines):

      HTTP/1.0 200 OK\r\n
      Content-Type: text/html; charset=UTF-8\r\n
      Content-Length: 119\r\n
      \r\n
 */
char *
build_response (char *uri, char *version, char **contents)
{
  int fd = open (uri, O_RDONLY);
  if (fd < 0)
    return NULL;
  
  struct stat file_info;
  if (fstat (fd, &file_info) < 0)
    {
      close (fd);
      return NULL;
    }
  
  size_t filesize = file_info.st_size;

  // Build the header
  char *header = strdup (version);
  if (!header) return NULL;  // Check allocation
  
  size_t headerlen = strlen (header);
  char *headers = " 200 OK\r\n"
    "Content-Type: text/html; charset=UTF-8\r\n"
    "Content-Length: ";
  
  headerlen += strlen(headers) + 1;
  header = realloc(header, headerlen);
  strncat(header, headers, strlen(headers));

  // Convert file size to string
  char size_as_string[21];
  snprintf(size_as_string, sizeof(size_as_string), "%ld", filesize);
	

	
  // Resize header again
  headerlen += strlen(size_as_string) + 2;  // Extra space for "\r\n\r\n"
  header = realloc(header, headerlen);
  strncat(header, size_as_string, strlen(size_as_string));
  strncat(header, "\r\n", 2);
   
  //add logic for HTTP/1.1 here- basically if it's HTTP/1.1 we need to add the connection:close at the end. Did it manually to pass tests
	if (!strcmp(version, "HTTP/1.0"))
		{
  		headerlen += strlen(size_as_string) + 2;  // Extra space for "\r\n\r\n"
  		header = realloc(header, headerlen);
  		strncat(header, "\r\n", 2);
  	}
  	if (!strcmp(version, "HTTP/1.1"))
		{
			char *closedConnection = "Connection: close\r\n\r\n";
			headerlen += strlen(closedConnection);
			header = realloc(header, headerlen);
			strncat(header, closedConnection, strlen(closedConnection));
		}

  // Allocate space for file contents
  *contents = malloc(filesize + 1);

  // Read file contents
  if (read(fd, *contents, filesize) == -1)
  {
    free(header);
    free(*contents);
    close(fd);
    return NULL;
  }
  
  close(fd);

  return header;  
}

