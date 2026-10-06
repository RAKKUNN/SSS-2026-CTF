/* [6주차 ROP] Level 1 (로컬 연습) — 단일 가젯으로 인자 1개 전달
 * 빌드(전 OS 동일): docker run --platform linux/amd64 -v $PWD:/w -w /w gcc:11 \
 *                   gcc -O0 -fno-stack-protector -no-pie -z noexecstack -o rop_intro rop_intro.c
 * 목표: RET을 덮어 pop rdi; ret 가젯으로 unlock(0xc0ffee) 호출 → 플래그 출력 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
void gadget(void){ __asm__ __volatile__("pop %rdi\n\tret\n\t"); }
void unlock(unsigned long key){
    if (key == 0xc0ffeeUL) { puts("[+] FLAG{rop_intro_practice}"); exit(0); }
    puts("[-] wrong key"); 
}
void vuln(void){
    char buf[64];
    printf(" [hint] buf    @ %p\n", (void*)buf);
    printf(" [hint] unlock @ %p\n", (void*)unlock);
    printf(" [hint] gadget @ %p (pop rdi; ret 는 내부에 있음 — objdump/ROPgadget)\n", (void*)gadget);
    printf("payload > "); fflush(stdout);
    read(0, buf, 256);
}
int main(void){ setvbuf(stdout,NULL,_IONBF,0); puts("[Level1] single-gadget ROP"); vuln(); return 0; }
