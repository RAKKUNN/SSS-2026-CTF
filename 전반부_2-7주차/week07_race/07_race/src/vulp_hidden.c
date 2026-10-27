/* vulp_hidden.c — 극소 윈도우 TOCTOU (Hidden)
 *
 * vulp.c 와 동일하나 access() 와 fopen() 사이에 sleep 이 없다.
 * race window 가 스케줄러 수준으로 최소라서, 비원자(unlink+symlink) 루프로는
 * 대상이 사라지는 순간(dangling)에 fopen 이 실패하거나 타이밍을 거의 못 맞춘다.
 * renameat2(RENAME_EXCHANGE) 로 두 심볼릭 링크를 '원자적으로' 교환하면
 * 링크가 절대 dangling 되지 않아 승률이 크게 올라간다. (SEED §3.5)
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(void) {
    char *fn = "/home/guest/work/ZZZ";
    char buffer[256];
    FILE *fp;

    if (scanf("%200s", buffer) != 1) return 1;

    if (access(fn, W_OK) == 0) {
        /* sleep 없음: access 와 fopen 사이 간극이 최소 */
        fp = fopen(fn, "a+");
        if (fp == NULL) return 1;
        fwrite("\n", sizeof(char), 1, fp);
        fwrite(buffer, sizeof(char), strlen(buffer), fp);
        fclose(fp);
    }
    return 0;
}
