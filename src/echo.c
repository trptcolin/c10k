#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <netdb.h>
#include <string.h>

void handle_connections_forever(int socket) {
  while(1) {
    struct sockaddr accepted_socket_address;
    socklen_t accepted_socket_address_length = sizeof(struct sockaddr);
    int accepted_fd = accept(socket, &accepted_socket_address, &accepted_socket_address_length);
    if(accepted_fd < 0) {
      perror("accept() error");
      exit(EXIT_FAILURE);
    }

    char input[1];
    int bytes_written;
    while(1 == read(accepted_fd, input, 1)) {
      bytes_written = write(accepted_fd, input, 1);
      if(bytes_written > 0) {
        printf("%c", input[0]);
      }
    }
    close(accepted_fd);
  }
}

void print_addrinfo_results_node(struct addrinfo* my_addrinfo_results_node) {
  char my_host_result[NI_MAXHOST];
  char my_port_result[NI_MAXSERV];
  int my_getname_result = getnameinfo(my_addrinfo_results_node->ai_addr,
      my_addrinfo_results_node->ai_addrlen,
      my_host_result,
      NI_MAXHOST,
      my_port_result,
      NI_MAXSERV,
      NI_NUMERICHOST | NI_NUMERICSERV
      );
  if(my_getname_result != 0) {
    perror("getnameinfo() error");
    exit(EXIT_FAILURE);
  }
  printf("host: '%s'\n", my_host_result);
  printf("port: '%s'\n", my_port_result);
}

int main(int argc, char* const argv[]) {
  struct addrinfo* my_addrinfo_results_head_node;

  char* requested_host;
  char* requested_port;

  if(argc == 2) {
    requested_host = "0.0.0.0";
    requested_port = argv[1];
  } else if (argc == 3) {
    requested_host = argv[1];
    requested_port = argv[2];
  } else {
    printf("Usage: echo [HOST] PORT_NUMBER\n");
    exit(EXIT_FAILURE);
  }

  struct addrinfo hints = {
    .ai_family = AF_UNSPEC,
    .ai_socktype = SOCK_STREAM,
    .ai_flags = AI_PASSIVE,
  };

  int getaddrinfo_result = getaddrinfo(requested_host, requested_port, &hints, &my_addrinfo_results_head_node);
  if(getaddrinfo_result != 0) {
    printf("Error parsing host/port: %s/%s: %s\n", requested_host, requested_port, gai_strerror(getaddrinfo_result));
    exit(EXIT_FAILURE);
  }

  struct addrinfo* my_addrinfo_results_node;

  int sock = 0;
  for(my_addrinfo_results_node = my_addrinfo_results_head_node;
      my_addrinfo_results_node != NULL;
      my_addrinfo_results_node = my_addrinfo_results_node->ai_next) {
    sock = socket(my_addrinfo_results_node->ai_family,
                  my_addrinfo_results_node->ai_socktype,
                  my_addrinfo_results_node->ai_protocol);

    int bind_result = bind(sock, my_addrinfo_results_node->ai_addr, my_addrinfo_results_node->ai_addrlen);
    if(bind_result < 0) {
      perror("bind() error");
      continue; // try another addrinfo result
    } else {
      print_addrinfo_results_node(my_addrinfo_results_node);
      break; // found a good one
    }
  }

  freeaddrinfo(my_addrinfo_results_head_node);

  if(sock == 0) {
    printf("socket wasn't bound\n");
    exit(EXIT_FAILURE);
  }

  int listen_result = listen(sock, 1);
  if(listen_result < 0) {
    perror("listen() error");
    exit(EXIT_FAILURE);
  }

  handle_connections_forever(sock);

  return EXIT_SUCCESS;
}
