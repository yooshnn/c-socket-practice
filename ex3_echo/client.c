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

  memset(&server_addr, 0, sizeof server_addr);
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(argv[1]);
  server_addr.sin_port = htons(atoi(argv[2]));

  if ((my_socket = socket(PF_INET, SOCK_STREAM, 0)) == -1) return 2;
  if (connect(my_socket, (struct sockaddr*) &server_addr, sizeof server_addr) == -1) return 3;

  int message_len;
  char buffer[1001];

  while (1) {
    printf("write your message. (q to quit)\n");

    fgets(buffer, 1001, stdin);
    if (strcmp(buffer, "q\n") == 0) break;

    message_len = strlen(buffer) - 1;
    buffer[message_len] = '\0';
    write(my_socket, buffer, message_len);

    while (message_len != 0) {
      int bytes_read = read(my_socket, buffer, sizeof buffer);
      
      message_len -= bytes_read;
      buffer[bytes_read] = '\0';

      printf("echo : %s\n", buffer);
    }
  }

  return 0;
}