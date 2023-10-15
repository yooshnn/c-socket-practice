#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main(int argc, char* argv[]) {
  if (argc != 3) {
    printf("usage : %s <port> <cycles>\n", argv[0]);
    return 1;
  }

  int server_socket, child_socket;
  struct sockaddr_in server_addr, child_addr;

  memset(&server_addr, 0, sizeof server_addr);
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(atoi(argv[1]));

  if ((server_socket = socket(PF_INET, SOCK_STREAM, 0)) == -1) return 2;
  if (bind(server_socket, (struct sockaddr*) &server_addr, sizeof server_addr) == -1) return 3;
  if (listen(server_socket, 5) == -1) return 4;

  for (int it = 1; it <= atoi(argv[2]); ++it) {
    int child_addr_len = sizeof child_addr;
    if ((child_socket = accept(server_socket, (struct sockaddr*) &child_addr, &child_addr_len)) == -1) return 5;

    printf("hello, client %d!\n", it);
    printf("  => client ip : %s\n         port : %d\n", inet_ntoa(child_addr.sin_addr), ntohs(child_addr.sin_port));

    int len;
    char buffer[21];

    while ((len = read(child_socket, buffer, 20)) > 0) {
      buffer[len] = '\0';
      printf("buffer : %s\n", buffer);
      write(child_socket, buffer, len);

      usleep(100);
    }

    printf("client %d closed connection.\n", it);

    close(child_socket);
  }
}