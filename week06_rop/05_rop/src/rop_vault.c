/*
 * [소프트웨어및시스템보안 2026] 6주차 CTF — Return-to-libc (ROP)
 *   Main   (argv[1]="main")  : libc 앵커(stdout) 힌트 제공 → system("/bin/sh") 체인 구성
 *   Hidden (argv[1]="hidden"): 힌트 없음 → puts(puts@got) 릭으로 libc base 복원 → 2단계 ret2libc
 * 보호기법: NX on / no canary / no-PIE  (ASLR는 커널 기본값=on)
 * 빌드    : gcc -O0 -fno-stack-protector -no-pie -z noexecstack
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* 정적 "/bin/sh" (no-PIE → 주소 고정, nm/objdump로 탐색 가능) */
char trusted_cmd[16] = "/bin/sh";

/* in-binary ROP 가젯: pop rdi; ret  (뒤이어 bare ret = 정렬용) */
void rop_gadgets(void) {
    __asm__ __volatile__(
        "pop %rdi\n\t"
        "ret\n\t"
    );
}

void banner(void) {
    puts("=====================================================");
    puts("        VaultGuard RTL Appliance  (Week06 ROP)       ");
    puts("=====================================================");
}

void vuln(int give_hint) {
    char buf[64];

    if (give_hint) {
        /* Main: libc 앵커(실제 libc를 가리키는 stdout FILE*) 1개 제공 */
        printf(" [hint] libc anchor (stdout) : %p\n", (void *)stdout);
        printf(" [hint] '/bin/sh' string     : %p\n", (void *)trusted_cmd);
    }
    printf("payload > ");
    fflush(stdout);
    read(0, buf, 256);            /* 취약점: 64바이트 버퍼에 256바이트 입력 → RET 오버플로우 */
}

int main(int argc, char *argv[]) {
    setvbuf(stdin,  NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    int give_hint = (argc > 1 && strcmp(argv[1], "main") == 0);

    banner();
    vuln(give_hint);             /* Hidden 2단계: 릭 후 main 으로 복귀 → 재진입 */
    return 0;
}
