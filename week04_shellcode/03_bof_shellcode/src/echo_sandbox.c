#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

void print_banner() {
    printf("=====================================================\n");
    printf("        EchoSandbox Shellcode Playground v3.1        \n");
    printf("=====================================================\n");
}

void mode_1_classic_shellcode() {
    char buf[64];
    memset(buf, 0, sizeof(buf));

    printf("\n--- Mode 1: Classic Stack Shellcode Execution (Main Flag) ---\n");
    printf(" [*] Target Stack Buffer Address : %p\n", buf);
    printf(" [*] Buffer Size                 : %zu bytes\n", sizeof(buf));
    printf(" [*] Maximum Read Size           : 256 bytes\n");
    printf("Enter shellcode payload > ");

    ssize_t n = read(0, buf, 256);
    (void)n;

    printf("[+] Payload received (%zd bytes). Returning to execute...\n", n);
}

int validate_strict_filter(const unsigned char *code, size_t len) {
    for (size_t i = 0; i < len; i++) {
        // Condition A: No NULL bytes allowed in shellcode
        if (code[i] == 0x00) {
            printf("[-] [FILTER REJECT] NULL byte (0x00) detected at offset %zu\n", i);
            return -1;
        }
        // Condition B: Forbidden plaintext strings (/bin/sh, sh)
        if (i + 7 <= len && memcmp(&code[i], "/bin/sh", 7) == 0) {
            printf("[-] [FILTER REJECT] Forbidden plaintext '/bin/sh' at offset %zu\n", i);
            return -1;
        }
        if (i + 3 <= len && memcmp(&code[i], "sh\0", 3) == 0) {
            printf("[-] [FILTER REJECT] Forbidden plaintext 'sh' at offset %zu\n", i);
            return -1;
        }
    }
    return 0;
}

void mode_2_restricted_shellcode() {
    char buf[256];
    memset(buf, 0, sizeof(buf));

    printf("\n--- Mode 2: Restricted Filter & XOR/ORW Shellcode (Hidden Flag) ---\n");
    printf(" [*] Filter Rules: Strict Null-Free (\\x00 Banned), No Plaintext '/bin/sh'\n");
    printf(" [*] Execution Buffer Address    : %p\n", buf);
    printf("Enter filtered shellcode > ");

    ssize_t n = read(0, buf, sizeof(buf));
    if (n <= 0) return;

    if (validate_strict_filter((const unsigned char *)buf, (size_t)n) != 0) {
        printf("[-] Payload rejected by security filter.\n");
        return;
    }

    printf("[+] Filter verification PASSED (%zd bytes)!\n", n);
    printf("[*] Transferring execution to shellcode buffer...\n");

    // Direct execution in executable stack
    void (*shellcode_fn)() = (void (*)())buf;
    shellcode_fn();
}

int main(int argc, char *argv[]) {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    print_banner();

    while (1) {
        printf("\nAvailable Options:\n");
        printf(" 1. Classic Shellcode Execution via BOF (Main Flag)\n");
        printf(" 2. Restricted Filter Shellcode Sandbox (Hidden Flag)\n");
        printf(" 3. Exit\n");
        printf("Choice > ");

        int choice = 0;
        if (scanf("%d", &choice) != 1) {
            break;
        }

        int c;
        while ((c = getchar()) != '\n' && c != EOF);

        switch (choice) {
            case 1:
                mode_1_classic_shellcode();
                break;
            case 2:
                mode_2_restricted_shellcode();
                break;
            case 3:
                printf("[*] Exiting EchoSandbox. Bye!\n");
                return 0;
            default:
                printf("[!] Invalid choice.\n");
                break;
        }
    }
    return 0;
}
