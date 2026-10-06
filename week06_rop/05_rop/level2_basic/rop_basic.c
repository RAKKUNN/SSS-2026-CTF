/* [6주차 ROP] Level 2 (로컬 연습) — 가젯 체이닝으로 함수 2개를 인자와 함께 연속 호출
 * 빌드: docker run --platform linux/amd64 -v $PWD:/w -w /w gcc:11 \
 *       gcc -O0 -fno-stack-protector -no-pie -z noexecstack -o rop_basic rop_basic.c
 * 목표: stageA(0xaaaa) → stageB(0xbbbb) 순서로 체이닝 → 플래그 출력 (원본 3.3 실습2 대응) */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int a_done = 0;
void gadget(void){ __asm__ __volatile__("pop %rdi\n\tret\n\t"); }
void stageA(unsigned long x){ if(x==0xaaaaUL){ a_done=1; puts("[A] ok"); } else { puts("[A] bad"); exit(1);} }
void stageB(unsigned long y){ if(a_done && y==0xbbbbUL){ puts("[+] FLAG{rop_basic_practice}"); exit(0);} puts("[B] bad"); }
void vuln(void){
    char buf[64];
    printf(" [hint] stageA @ %p\n", (void*)stageA);
    printf(" [hint] stageB @ %p\n", (void*)stageB);
    printf(" [hint] gadget @ %p\n", (void*)gadget);
    printf("payload > "); fflush(stdout);
    read(0, buf, 256);
}
int main(void){ setvbuf(stdout,NULL,_IONBF,0); puts("[Level2] gadget chaining"); vuln(); return 0; }
