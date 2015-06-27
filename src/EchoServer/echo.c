#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "tcp_server.h"

static void handle_accepted_socket(int accepted_fd) {
  char input[1];
  ssize_t bytes_written;
  while(1 == read(accepted_fd, input, 1)) {
    bytes_written = write(accepted_fd, input, 1);
    if(bytes_written > 0) {
      printf("%c", input[0]);
    }
  }
  close(accepted_fd);
}

int main(int argc, char* const argv[]) {
  char* requested_host = NULL;
  char* requested_port = NULL;

  if(argc == 2) {
    requested_host = "0.0.0.0";
    requested_port = argv[1];
  } else if (argc == 3) {
    requested_host = argv[1];
    requested_port = argv[2];
  } else {
    fprintf(stderr, "Usage: echo [HOST] PORT_NUMBER\n");
    return EXIT_FAILURE;
  }

  return run_tcp_server(requested_host,
                        requested_port,
                        handle_accepted_socket);
}
