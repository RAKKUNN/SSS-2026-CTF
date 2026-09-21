#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    char shellcode[128];

    printf("=====================================================\n");
    printf("     [Shellcode Level 1: Intro] Direct Shellcode Exec \n");
    printf("=====================================================\n");
    printf(" [*] Shellcode Buffer Address : %p\n", shellcode);
    printf("Enter raw shellcode bytes (e.g. 24B execve) > ");

    ssize_t n = read(0, shellcode, sizeof(shellcode));
    if (n <= 0) return 1;

    printf("[+] Executing %zd bytes of shellcode directly...\n", n);

    // Direct invocation via function pointer
    void (*func)() = (void (*)())shellcode;
    func();

    return 0;
}
