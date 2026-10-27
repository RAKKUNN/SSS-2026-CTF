#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile int target = 0;

void print_flag() {
    printf("\n[+] ==================================================\n");
    printf("[+] [SUCCESS] target variable corrupted to 4!\n");
    printf("[+] FLAG{fsb_basic_practice}\n");
    printf("[+] ==================================================\n\n");
}

int main() {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    char memo[64];

    printf("=====================================================\n");
    printf("     [FSB Level 2: Basic] Arbitrary Write (%%n)       \n");
    printf("=====================================================\n");
    printf(" [*] Target Variable Address (&target) : %p\n", (void *)&target);
    printf(" [*] Initial Target Value              : %d (Need: 4)\n", target);
    printf("Enter memo > ");

    read(0, memo, sizeof(memo) - 1);

    printf("Echo: ");
    printf(memo);
    printf("\n");

    printf(" [*] Result Target Value: %d\n", target);

    if (target == 4) {
        print_flag();
    } else {
        printf("[-] target is not 4. Use %%n or %%4c to write exact byte count!\n");
    }
    return 0;
}
