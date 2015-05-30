#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>

uint16_t parse_port(char* port_string) {
  // TODO: this needs error checking to be robust
  return strtol(port_string, NULL, 10);
}

struct sockaddr_in make_socket_address_from_strings(char* address_str, char* port_str) {
  struct sockaddr_in address;

  address.sin_family = AF_INET;
  uint16_t port = parse_port(port_str);
  address.sin_port = htons(port);

  int result = inet_aton(address_str, &address.sin_addr);
  switch(result) {
    case 1: break;
    case 0: printf("Could not parse address: \"%s\"\n", address_str);
            exit(1);
            break;
    case -1: perror("Error attempting to parse address");
             exit(1);
             break;
  }

  return address;
}

void handle_connections_forever(int socket) {
  while(1) {
    struct sockaddr accepted_socket_address;
    socklen_t accepted_socket_address_length = sizeof(struct sockaddr_in);
    int accepted_fd = accept(socket, &accepted_socket_address, &accepted_socket_address_length);
    if(accepted_fd < 0) {
      perror("accept() error");
      exit(1);
    }

    char input[1];
    while(1 == read(accepted_fd, input, 1)) {
      write(accepted_fd, input, 1);
      printf("%c", input[0]);
    }
    close(accepted_fd);
  }
}

int main(int argc, char* const argv[]) {
  struct sockaddr_in address;
  if(argc == 2) {
    address = make_socket_address_from_strings("0.0.0.0", argv[1]);
  } else if (argc == 3) {
    address = make_socket_address_from_strings(argv[1], argv[2]);
  } else {
    printf("Usage: echo [ADDRESS] PORT_NUMBER\n");
    exit(1);
  }

  int sock = socket(AF_INET, SOCK_STREAM, 0);

  int bind_result = bind(sock, (struct sockaddr*) &address, sizeof(struct sockaddr_in));
  if(bind_result < 0) {
    perror("bind() error");
    exit(1);
  }

  int listen_result = listen(sock, 1);
  if(listen_result < 0) {
    perror("listen() error");
    exit(1);
  }

  const struct sockaddr_in bound_address;
  socklen_t bound_address_length = sizeof(struct sockaddr_in);
  int result = getsockname(sock, (struct sockaddr*) &bound_address, &bound_address_length);

  char* bound_address_str = inet_ntoa(bound_address.sin_addr);
  uint16_t bound_port = ntohs(bound_address.sin_port);
  printf("Listening on: %s:%d\n", bound_address_str, bound_port);

  handle_connections_forever(sock);
}
