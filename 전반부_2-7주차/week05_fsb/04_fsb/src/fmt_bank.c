#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>

// Global target variables in fixed non-PIE memory addresses
uint64_t secret_vault_key = 0x5359534c41423236ULL; // "SYSLAB26"
volatile uint32_t is_vip_manager = 0x00000000;

void print_banner() {
    printf("=====================================================\n");
    printf("           VaultCorp International Banking v4.2      \n");
    printf("=====================================================\n");
    printf(" [*] Target Variable &is_vip_manager : %p\n", (void *)&is_vip_manager);
    printf(" [*] Secret Key Target &secret_key   : %p\n", (void *)&secret_vault_key);
    printf("=====================================================\n");
}

void print_main_flag() {
    FILE *fp = fopen("/home/pwn/flag_main.txt", "r");
    if (!fp) fp = fopen("flag_main.txt", "r");
    if (!fp) {
        printf("[!] Error: flag_main.txt missing.\n");
        return;
    }
    char flag[128];
    if (fgets(flag, sizeof(flag), fp)) {
        printf("\n[+] ==================================================\n");
        printf("[+] [SUCCESS] Vault Key Accepted! Main Flag:\n");
        printf("[+] %s", flag);
        printf("[+] ==================================================\n\n");
    }
    fclose(fp);
}

void print_hidden_flag() {
    FILE *fp = fopen("/home/pwn/flag_hidden.txt", "r");
    if (!fp) fp = fopen("flag_hidden.txt", "r");
    if (!fp) {
        printf("[!] Error: flag_hidden.txt missing.\n");
        return;
    }
    char flag[128];
    if (fgets(flag, sizeof(flag), fp)) {
        printf("\n[+] ==================================================\n");
        printf("[+] [CONGRATULATIONS] VIP Manager Authorization Verified!\n");
        printf("[+] Hidden Flag: %s", flag);
        printf("[+] ==================================================\n\n");
    }
    fclose(fp);
}

void handle_feedback() {
    char memo[256];
    memset(memo, 0, sizeof(memo));

    printf("\n--- Mode 1: Customer Feedback (Format String Interface) ---\n");
    printf("Enter memo / feedback > ");

    ssize_t n = read(0, memo, sizeof(memo) - 1);
    if (n <= 0) return;

    printf("\n[Receipt] ");
    // Vulnerability: Direct user-controlled format string without format specifier
    printf("%s", ""); // dummy to satisfy some linters
    printf(memo);
    printf("\n");
}

void check_vip_status() {
    printf("\n--- Mode 2: VIP Manager Dashboard ---\n");
    printf(" [*] Current is_vip_manager value: 0x%08x (Required: 0x1337beef)\n", is_vip_manager);

    if (is_vip_manager == 0x1337beef) {
        printf("[+] Authorization Accepted! Welcome, Executive Manager.\n");
        print_hidden_flag();
    } else {
        printf("[-] Access Denied: Invalid VIP manager status.\n");
    }
}

void unlock_vault() {
    printf("\n--- Mode 3: Encrypted Vault Key Unlock ---\n");
    printf("Enter 64-bit Hex Secret Key (e.g., 0x1122334455667788) > ");

    uint64_t entered_key = 0;
    if (scanf("%" SCNx64, &entered_key) != 1) {
        printf("[-] Parsing error.\n");
        return;
    }

    if (entered_key == secret_vault_key) {
        printf("[+] Secret key matches master vault record!\n");
        print_main_flag();
    } else {
        printf("[-] Authentication failed. Key 0x%016" PRIx64 " does not match.\n", entered_key);
    }
}

int main(int argc, char *argv[]) {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    print_banner();

    while (1) {
        printf("\nOperations Menu:\n");
        printf(" 1. Submit Feedback / Memo (Format String)\n");
        printf(" 2. Check VIP Manager Dashboard (Hidden Flag)\n");
        printf(" 3. Unlock Vault with Secret Key (Main Flag)\n");
        printf(" 4. Exit\n");
        printf("Choice > ");

        int choice = 0;
        if (scanf("%d", &choice) != 1) {
            break;
        }

        int c;
        while ((c = getchar()) != '\n' && c != EOF);

        switch (choice) {
            case 1:
                handle_feedback();
                break;
            case 2:
                check_vip_status();
                break;
            case 3:
                unlock_vault();
                break;
            case 4:
                printf("[*] Session terminated. Goodbye!\n");
                return 0;
            default:
                printf("[!] Invalid option.\n");
                break;
        }
    }
    return 0;
}
