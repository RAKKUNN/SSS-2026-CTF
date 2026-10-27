#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>

int main() {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    volatile uint64_t secret_token = 0x1337cafebeefdeadULL;
    char name[64];

    printf("=====================================================\n");
    printf("     [FSB Level 1: Intro] Stack Memory Reader (%%p)   \n");
    printf("=====================================================\n");
    printf("Enter your name / memo > ");

    read(0, name, sizeof(name) - 1);

    printf("Hello: ");
    printf(name);
    printf("\n");

    printf("Enter the 64-bit secret token in hex (without 0x) > ");
    uint64_t entered = 0;
    if (scanf("%" SCNx64, &entered) == 1) {
        if (entered == secret_token) {
            printf("\n[+] [SUCCESS] Correct secret leaked!\n");
            printf("[+] FLAG{fsb_intro_practice}\n\n");
        } else {
            printf("[-] Wrong secret token: 0x%016" PRIx64 "\n", entered);
        }
    }
    return 0;
}
