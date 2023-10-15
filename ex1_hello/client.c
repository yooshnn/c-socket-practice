#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main(int argc, char* argv[]) {
  /* arg 수 확인 */
  if (argc != 3) {
    printf("usage : %s <IP> <port>\n", argv[0]);
    return 1;
  }

  /* 필요한 변수들 선언 */
  int my_socket; // 소켓의 file descriptor
  struct sockaddr_in server_addr; // IPv4 주소 정보

  int len; // read()의 반환 값 저장 (-1: err, 0: eof, otherwise 읽은 바이트 개수)
  char buffer[1001]; // 최대 1000개의 문자를 담는 버퍼

  /* 소켓의 프로토콜 정보 설정하기 */
  my_socket = socket(PF_INET, SOCK_STREAM, 0);
  if (my_socket == -1) {
    return 2;
  }

  /* 주소 정보 설정하기 */
  memset(&server_addr, 0, sizeof server_addr);
  server_addr.sin_family = AF_INET; // IPv4
  server_addr.sin_addr.s_addr = inet_addr(argv[1]); // w.x.y.z 형태의 IPv4 주소를 uint_32로 변환
  server_addr.sin_port = htons(atoi(argv[2])); // 포트 번호

  /* 연결 시도 (SYN 전송) */
  if (connect(my_socket, (struct sockaddr*) &server_addr, sizeof server_addr) == -1) {
    return 3;
  }

  /* 서버로부터 메시지 받기 */
  int cur = 0;
  do {
    if ((len = read(my_socket, buffer + cur, (sizeof buffer) - 1 - cur)) == -1) {
      return 4;
    }
    printf("len : %d\n", len);
  } while (len > 0);

  /* 받은 메시지를 출력 후 통신 종료 */
  printf("Server's message : %s\n", buffer);
  close(my_socket);

  return 0;
}