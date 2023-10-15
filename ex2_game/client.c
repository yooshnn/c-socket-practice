#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main(int argc, char* argv[]) {
  if (argc != 3) {
    printf("usage : %s <IP> <port>\n", argv[0]);
    return 1;
  }

  int my_socket;
  struct sockaddr_in server_addr;

  int len;
  char buffer[101];

  my_socket = socket(PF_INET, SOCK_STREAM, 0);
  if (my_socket == -1) {
    return 2;
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(argv[1]);
  server_addr.sin_port = htons(atoi(argv[2]));

  if (connect(my_socket, (struct sockaddr*) &server_addr, sizeof server_addr) == -1) {
    return 3;
  }

  do {
    len = read(my_socket, buffer, (sizeof buffer) - 1);

    if (len == -1) {
      close(my_socket);
      return 4;
    }
    if (len == 0) {
      close(my_socket);
      break;
    }

    buffer[len] = '\0';
    printf("@ %s (%d)\n", buffer, len);

    switch (buffer[1]) {
      case '1': {
        fgets(buffer, 51, stdin);
        buffer[strlen(buffer) - 1] = '\0';

        write(my_socket, buffer, strlen(buffer));
        break;
      }
      case '2': {
        // wait for opponent
        break;
      }
      default: {
        // game over, next read must return EOF
      }
    }
  } while (len > 0);

  return 0;
}