#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void host_game(int* fds);

int main(int argc, char* argv[]) {
  if (argc != 2) {
    printf("usage : %s <port>\n", argv[0]);
    return 1;
  }

  // 필요한 변수 선언
  int server_socket, child_socket[2];
  struct sockaddr_in server_addr, child_addr[2];

  // 서버 소켓의 프로토콜 정보 설정
  server_socket = socket(PF_INET, SOCK_STREAM, 0);
  if (server_socket == -1) return 2;

  // 서버 소켓이 사용할 주소 정보 설정
  memset(&server_addr, 0, sizeof server_addr);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(atoi(argv[1]));
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  // 서버 소켓에 주소 정보 bind
  if (bind(server_socket, (struct sockaddr*) &server_addr, sizeof server_addr) == -1) {
    return 3;
  }

  // 서버 소켓을 listen 상태로 전환
  if (listen(server_socket, 5) == -1) {
    return 4;
  }

  while (1) {
    for (int i = 0; i < 2; ) {
      int child_addr_len = sizeof child_addr[i];
      child_socket[i] = accept(server_socket, (struct sockaddr*) &child_addr[i], &child_addr_len);
      
      if (child_socket[i] != -1) {
        if (i == 0) {
          char message[51] = "please wait until we find an opponent.";
          write(child_socket[0], message, strlen(message));
        }
        i += 1;
      }
    }

    switch (fork()) {
      case -1: { // FORK FAILED
        close(child_socket[0]);
        close(child_socket[1]);
        return 5;
      }
      case 0: { // child process
        close(server_socket); // decrease server_socket FD reference count

        host_game(child_socket);

        close(child_socket[0]);
        close(child_socket[1]);

        printf("the clients disconnected.\n");
        return 0;
      }
      default: { // parent process
        close(child_socket[0]);
        close(child_socket[1]);

        printf("parent process is waiting.\n");
        wait((int*)0);
        printf("parent process has done waiting.\n");
      }
    }
  }
}

void host_game(int* fd) {
  char words[1000][51];
  int winner = -1, turn = 0;

  int len = 0;
  char buffer[51];

  char message1[101], message2[101];

  strcpy(message1, "[1] please type a word. => ");
  strcpy(message2, "[2] please wait for your opponent.");
  write(fd[0], message1, strlen(message1));
  write(fd[1], message2, strlen(message2));

  while (winner == -1) {
    // assume that read always reads the full message
    // & participants always write a valid word (len <= 50)
    len = read(fd[turn & 1], buffer, (sizeof buffer) - 1);
    if (len == -1) { // lose if error
      winner = (~turn) & 1;
      break;
    }
    buffer[len] = '\0';

    printf("participant %d sent %s (%d)\n", turn & 1, buffer, strlen(buffer));
    strcpy(words[turn], buffer);

    int ok = (turn == 0 || buffer[0] == words[turn - 1][strlen(words[turn - 1]) - 1]);
    ok &= (len > 1);
    for (int i = 0; ok && (i < turn); ++i) ok &= (strcmp(buffer, words[i]) != 0);

    if (ok) {
      strcpy(message1, "[2] ok! please wait for your opponent.");
      strcpy(message2, "[1] please type a word. => ");
      strcat(message2, buffer);
      
      write(fd[turn & 1], message1, strlen(message1));
      write(fd[(~turn) & 1], message2, strlen(message2));

      turn += 1;
    } else {
      winner = (~turn) & 1;
    }
  }

  strcpy(message1, "[3] game over! you lose.");
  strcpy(message2, "[3] game over! you win. => ");
  strcat(message2, buffer);
  
  write(fd[turn & 1], message1, strlen(message1));
  write(fd[(~turn) & 1], message2, strlen(message2));
}