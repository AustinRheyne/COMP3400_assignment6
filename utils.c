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

  if (getaddrinfo (NULL, protocol, &hints, &server_list) != 0)
    return -1;

  // Traverse through the server list, trying to create and bind a socket,
  // similar to how a client would work.
  int socketfd = -1;
  struct addrinfo *server = NULL;
  for (server = server_list; server != NULL; server = server->ai_next)
    {
      if ((socketfd = socket (server->ai_family, server->ai_socktype, 0)) < 0)
        continue;

      // TODO: Set the socket options and bind it to the server->ai_addr.
      // You should set SO_REUSEADDR socket_option and SO_RCVTIMEO to the
      // timeout provided. You can adjust the timeout if desired; the first
      // array element is seconds, and the second is milliseconds. If the
      // bind is successful, you should break out of this for-loop. Otherwise,
      // shutdown and close the socket and set socketfd back to -1; the loop
      // will continue, trying with the next addrinfo entry.
      int socket_option = -1;
      struct timeval timeout = { 10, 0 }; // 10.0 seconds
    }

  // TODO: Free the server list and convert the socket to a server socket.
  // At this point, we either have a socket (socketfd > 0) or we don't
  // (socketfd == -1). Either way, we do not need the list of addrinfo
  // structs any more, so free them using freeaddrinfo(). If we do have
  // a socket, use listen() to convert it to a server socket.

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

  // TODO: Accept the connection request and return the specified values
  // described above.
  
  return "";
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
  // Open the file for reading and get the file size.
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

  // TODO: For FULL requirements, don't close the file this soon, as you
  // will need to read in the file contents.
  close (fd);

  // TODO: Build the header by resizing and concatenating the strings as
  // needed. See the sample code on the assignment description.
  char *header = strdup (version);
  assert (header != NULL);
  size_t headerlen = strlen (header);
  char *headers = " 200 OK\r\n"
    "Content-Type: text/html; charset=UTF-8\r\n"
    "Content-Length: ";
  // Create a 21-character buffer to store the file size as a string (use
  // snprintf() to convert the size_t into a char*). We can safely assume the
  // string version while fit, as size_t is a 64-bit value that has a maximum
  // value of 18,446,744,073,709,551,615.
  char size_as_string[21];
  memset (size_as_string, 0, 21);

  // TODO: Reallocate the header pointer to append the file's size (as a
  // content-length string) and the CRLF characters. Use the realloc-strncat
  // model shown above. For FULL requirements, also append the
  // "Connection: close\r\n" for HTTP/1.1 requests and the file contents.

  // TODO: For FULL requirements, replace this string with the contents
  // read in from the file:
  *contents = "<html>\n  <head>\n    <title>Success!</title>\n"
    "  </head>\n\n  <body>\n    <h2>It <i>really</i> "
    "works!</h2>\n  </body>\n</html>\n";
  
  return NULL;
}
