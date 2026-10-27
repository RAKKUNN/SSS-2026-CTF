/* vulp.c — setuid-root TOCTOU 취약 프로그램 (Main)
 *
 * access()는 '실제 uid(real uid = guest)' 기준으로 쓰기 권한을 확인하고,
 * fopen()은 '유효 uid(effective uid = root, setuid 때문)' 로 실제 파일을 연다.
 * 이 둘 사이의 시간 간격(race window)에 /home/guest/work/XYZ 가 가리키는 대상이
 * 바뀌면, guest 가 쓸 수 없어야 할 파일(/etc/passwd)에 root 권한으로 쓰게 된다.
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(void) {
    char *fn = "/home/guest/work/XYZ";
    char buffer[256];
    FILE *fp;

    if (scanf("%200s", buffer) != 1) return 1;

    /* [CHECK] access() : 실행자(real uid = guest)가 쓸 수 있는가? */
    if (access(fn, W_OK) == 0) {
        usleep(30000);              /* race window (~30ms) : 루프 공격이 승리할 여지 */
        fp = fopen(fn, "a+");       /* [USE] effective uid = root 로 실제 파일에 씀 */
        if (fp == NULL) { printf("open failed\n"); return 1; }
        fwrite("\n", sizeof(char), 1, fp);
        fwrite(buffer, sizeof(char), strlen(buffer), fp);
        fclose(fp);
        printf("appended\n");
    } else {
        printf("No permission\n");
    }
    return 0;
}
