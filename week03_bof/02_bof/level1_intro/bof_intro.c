#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void print_flag() {
    printf("\n[+] ==================================================\n");
    printf("[+] [SUCCESS] You modified is_admin! Level 1 Complete!\n");
    printf("[+] FLAG{bof_intro_practice}\n");
    printf("[+] ==================================================\n\n");
}

int main() {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    volatile int is_admin = 0;
    char buffer[16];

    printf("=====================================================\n");
    printf("     [BOF Level 1: Intro] Stack Variable Overwrite   \n");
    printf("=====================================================\n");
    printf(" [*] Address of buffer   : %p\n", buffer);
    printf(" [*] Address of is_admin : %p\n", (void *)&is_admin);
    printf(" [*] Current is_admin    : %d (Need: Non-zero)\n", is_admin);
    printf("Enter username > ");

    read(0, buffer, 64);

    printf(" [*] Result is_admin     : %d (0x%08x)\n", is_admin, is_admin);

    if (is_admin != 0) {
        print_flag();
    } else {
        printf("[-] is_admin is still 0. Try overflowing buffer!\n");
    }
    return 0;
}
