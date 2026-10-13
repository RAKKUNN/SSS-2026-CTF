/* vulp_practice.c — L1 연습: 넓은 윈도우(sleep 10)로 TOCTOU 를 눈으로 관찰한다.
 *
 * setuid 가 필요 없는 연습판: 자기 소유 디렉토리에서 ./target 과 ./secret_prize 를
 * 두고, access 통과 후 10초 동안 심볼릭 링크를 secret_prize 로 바꾸면 fopen 이
 * secret_prize 에 쓰는 것을 확인한다.
 *
 * 빌드:  gcc -o vulp_practice vulp_practice.c
 * 준비:  touch secret_prize ; ln -sf target_file target ; (target_file 도 touch)
 * 실행 중 다른 터미널에서:  ln -sf secret_prize target
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(void) {
    char *fn = "./target";
    char buf[128];
    FILE *fp;

    printf("[*] access() 확인 중...\n");
    if (access(fn, W_OK) == 0) {
        printf("[*] 지금부터 10초! 다른 터미널에서 'ln -sf secret_prize target' 실행\n");
        sleep(10);
        fp = fopen(fn, "a+");
        if (fp == NULL) { printf("open 실패\n"); return 1; }
        if (fgets(buf, sizeof(buf), stdin) != NULL)
            fwrite(buf, 1, strlen(buf), fp);
        fclose(fp);
        printf("[+] 기록 완료. secret_prize 내용을 확인하세요.\n");
    } else {
        printf("No permission\n");
    }
    return 0;
}
