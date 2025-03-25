#include <arpa/inet.h>
#include <assert.h>
#include <check.h>
#include <fcntl.h>
#include <inttypes.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../utils.h"

START_TEST (MIN_build_response_header)
{
  printf ("======================================\n");
  printf ("PUBLIC\n");
  printf ("  MIN build response header\n");
  printf ("  Given uri=srv_root/index.html and version=HTTP/1.0,\n");
  printf ("  build the response (contents ignored):\n\n");
  printf ("    HTTP/1.0 200 OK\\r\\n\n");
  printf ("    Content-Type: text/html; charset=UTF-8\\r\\n\n");
  printf ("    Content-Length: 119\\r\\n\n");
  printf ("    \\r\\n\n\n");
  char *contents = NULL; // IGNORED
  char *header = build_response ("srv_root/index.html", "HTTP/1.0",
                                 &contents);
  char *expected = "HTTP/1.0 200 OK\r\n"
    "Content-Type: text/html; charset=UTF-8\r\n"
    "Content-Length: 119\r\n"
    "\r\n";
  ck_assert_str_eq (header, expected);
  free (header);
}
END_TEST

START_TEST (FULL_build_response_header_v1)
{
  printf ("======================================\n");
  printf ("PUBLIC\n");
  printf ("  FULL build response header\n");
  printf ("  Given uri=srv_root/index.html and version=HTTP/1.1,\n");
  printf ("  build the response (contents NOT ignored):\n\n");
  printf ("    HTTP/1.1 200 OK\\r\\n\n");
  printf ("    Content-Type: text/html; charset=UTF-8\\r\\n\n");
  printf ("    Content-Length: 119\\r\\n\n");
  printf ("    \\r\\n\n\n");
  char *contents = NULL; // IGNORED
  char *header = build_response ("srv_root/index.html", "HTTP/1.1",
                                 &contents);
  char *expected = "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html; charset=UTF-8\r\n"
    "Content-Length: 119\r\n"
    "Connection: close\r\n"
    "\r\n";
  ck_assert_str_eq (header, expected);
  free (header);
  char *html = "<html>\n"
    "  <head>\n"
    "    <title>Success!</title>\n"
    "  </head>\n"
    "\n"
    "  <body>\n"
    "    <h2>It <i>really</i> works!</h2>\n"
    "  </body>\n"
    "</html>\n";
  ck_assert_str_eq (contents, html);
}
END_TEST

START_TEST (FULL_build_response_index)
{
  printf ("======================================\n");
  printf ("PUBLIC\n");
  printf ("  FULL build response header index\n");
  printf ("  Given uri=srv_root/index.html and version=HTTP/1.0,\n");
  printf ("  build the response:\n\n");
  printf ("    HTTP/1.1 200 OK\\r\\n\n");
  printf ("    Content-Type: text/html; charset=UTF-8\\r\\n\n");
  printf ("    Content-Length: 119\\r\\n\n");
  printf ("    \\r\\n\n\n");
  printf ("  And set contents to:\n\n");
  printf ("    <html>\n");
  printf ("      <head>\n");
  printf ("        <title>Success!</title>\n");
  printf ("      </head>\n");
  printf ("    \n");
  printf ("      <body>\n");
  printf ("        <h2>It <i>really</i> works!</h2>\n");
  printf ("      </body>\n");
  printf ("    </html>\n\n");
  char *contents = NULL;
  char *header = build_response ("srv_root/index.html", "HTTP/1.0",
                                 &contents);
  char *expected = "HTTP/1.0 200 OK\r\n"
    "Content-Type: text/html; charset=UTF-8\r\n"
    "Content-Length: 119\r\n"
    "\r\n";
  ck_assert_str_eq (header, expected);
  free (header);
  char *html = "<html>\n"
    "  <head>\n"
    "    <title>Success!</title>\n"
    "  </head>\n"
    "\n"
    "  <body>\n"
    "    <h2>It <i>really</i> works!</h2>\n"
    "  </body>\n"
    "</html>\n";
  ck_assert_str_eq (contents, html);
  printf ("  WARNING: If this free() fails, it is because you did not\n");
  printf ("  dynamically allocate and read in the file contents. Instead,\n");
  printf ("  you still have a hard-coded contents string in build_response()\n");
  printf ("  that needs replaced.\n");
  free (contents);
}
END_TEST

void public_tests (Suite *s)
{
  TCase *tc_public = tcase_create ("Public");
  tcase_add_test (tc_public, MIN_build_response_header);
  tcase_add_test (tc_public, FULL_build_response_header_v1);
  tcase_add_test (tc_public, FULL_build_response_index);
  suite_add_tcase (s, tc_public);
}

