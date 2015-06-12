#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <netdb.h>
#include <string.h>

void handle_accepted_socket(int accepted_fd) {
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

void accept_connections_forever(int socket, void(*handle_accepted)(int)) {
  while(1) {
    struct sockaddr accepted_socket_address;
    socklen_t accepted_socket_address_length = sizeof(struct sockaddr);
    int accepted_fd = accept(socket,
                             &accepted_socket_address,
                             &accepted_socket_address_length);
    if(accepted_fd < 0) {
      perror("accept(2) error");
      continue;
    }

    handle_accepted(accepted_fd);
    close(accepted_fd);
  }
}

void print_addrinfo(struct addrinfo* addrinfo) {
  char host[NI_MAXHOST];
  char port[NI_MAXSERV];
  int my_getname_result = getnameinfo(addrinfo->ai_addr,
                                      addrinfo->ai_addrlen,
                                      host,
                                      NI_MAXHOST,
                                      port,
                                      NI_MAXSERV,
                                      NI_NUMERICHOST | NI_NUMERICSERV);
  if(my_getname_result != 0) {
    perror("getnameinfo(3) error");
  } else {
    printf("host/port: '%s/%s'\n", host, port);
  }
}

int bind_socket(const char* host,
                const char* port,
                struct addrinfo* hints) {
  const int error = -1;

  struct addrinfo* addrinfo_results_head;
  int getaddrinfo_result = getaddrinfo(host,
                                       port,
                                       hints,
                                       &addrinfo_results_head);
  if(getaddrinfo_result != 0) {
    const char* error_string = gai_strerror(getaddrinfo_result);
    fprintf(stderr,
            "Error parsing host/port: %s/%s: %s\n",
            host,
            port,
            error_string);
    return error;
  } else if (addrinfo_results_head == NULL) {
    fprintf(stderr, "No match found for host/port: %s/%s\n", host, port);
    return error;
  }

  struct addrinfo* my_addrinfo_results_node;
  int sock;
  for(my_addrinfo_results_node = addrinfo_results_head;
      my_addrinfo_results_node != NULL;
      my_addrinfo_results_node = my_addrinfo_results_node->ai_next) {

    sock = socket(my_addrinfo_results_node->ai_family,
                  my_addrinfo_results_node->ai_socktype,
                  my_addrinfo_results_node->ai_protocol);
    if(sock < 0) {
      perror("socket(2) error");
      return error;
    }

    int bind_result = bind(sock,
                           my_addrinfo_results_node->ai_addr,
                           my_addrinfo_results_node->ai_addrlen);
    if(bind_result < 0) {
      perror("bind(2) error");
      continue; // try another addrinfo result
    } else {
      print_addrinfo(my_addrinfo_results_node);
      break; // found a good one
    }
  }

  freeaddrinfo(addrinfo_results_head);
  return sock;
}

int run_tcp_server(const char* requested_host,
                   const char* requested_port,
                   void(*handle_accepted)(int)) {
  struct addrinfo hints = {
    .ai_family = AF_UNSPEC,
    .ai_socktype = SOCK_STREAM,
    .ai_flags = AI_PASSIVE,
  };

  int sock = bind_socket(requested_host, requested_port, &hints);
  if(sock < 0) {
    fprintf(stderr, "Socket could not be bound\n");
    return EXIT_FAILURE;
  }

  int listen_result = listen(sock, 1);
  if(listen_result < 0) {
    perror("listen(2) error");
    return EXIT_FAILURE;
  }

  accept_connections_forever(sock, handle_accepted);

  return EXIT_SUCCESS;
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
