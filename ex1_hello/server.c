#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main(int argc, char* argv[]) {
  /* arg 수 확인 */
  if (argc != 2) {
    printf("usage : %s <port>\n", argv[0]);
    return 1;
  }

  /* 필요한 변수 선언 */
  int server_socket, child_socket; // 소켓의 file descriptor
  struct sockaddr_in server_addr, child_addr; // IPv4 주소 정보

  char* message = "Hello, world!";

  /* 소켓의 프로토콜 정보 설정하기 */
  server_socket = socket(PF_INET, SOCK_STREAM, 0);
  if (server_socket == -1) {
    return 2;
  }

  /* 주소 정보 설정하기 */
  memset(&server_addr, 0, sizeof server_addr);
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY : 사용 가능한 아무 IP 주소
  server_addr.sin_port = htons(atoi(argv[1]));

  /* 소켓에 주소 정보 넣기 */
  if (bind(server_socket, (struct sockaddr*) &server_addr, sizeof server_addr) == -1) {
    return 3;
  }

  /* 소켓을 서버 소켓으로 전환하기 (SYN+ACK 전송이 가능해짐) */
  if (listen(server_socket, 5) == -1) {
    return 4;
  }

  /* accept 호출 (block) */
  int child_addr_len = sizeof child_addr; // accept 함수의 
  if ((child_socket = accept(server_socket, (struct sockaddr*) &child_addr, &child_addr_len)) == -1) {
    return 5;
  }

  /* 메시지 전송 후 통신 종료 */
  write(child_socket, message, strlen(message));

  close(child_socket);
  close(server_socket);

  return 0;
}