#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void vuln() {
    char buf[64];
    printf(" [*] Stack Buffer Address : %p\n", buf);
    printf("Enter payload > ");
    read(0, buf, 128);
}

int main() {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    printf("=====================================================\n");
    printf("     [Shellcode Level 2: Basic] Stack Shellcode Jump \n");
    printf("=====================================================\n");
    vuln();
    printf("[-] Returned safely without shellcode trigger.\n");
    return 0;
}
