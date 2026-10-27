#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void win() {
    printf("\n[+] ==================================================\n");
    printf("[+] [SUCCESS] Execution flow hijacked to win()!\n");
    printf("[+] FLAG{bof_basic_practice}\n");
    printf("[+] ==================================================\n\n");
    exit(0);
}

void vulnerable_function() {
    char buffer[32];
    printf(" [*] Buffer Address : %p\n", buffer);
    printf(" [*] win() Function : %p\n", win);
    printf("Enter payload > ");
    read(0, buffer, 128);
}

int main() {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    printf("=====================================================\n");
    printf("     [BOF Level 2: Basic] Return Address Overwrite   \n");
    printf("=====================================================\n");
    vulnerable_function();
    printf("[-] Returned safely without hijacking. Try again!\n");
    return 0;
}
