#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>

void print_banner() {
    printf("=====================================================\n");
    printf("           VaultGuard Security Appliance v2.6        \n");
    printf("=====================================================\n");
}

void print_main_flag() {
    FILE *fp = fopen("/home/pwn/flag_main.txt", "r");
    if (!fp) {
        // Fallback for local debugging
        fp = fopen("flag_main.txt", "r");
    }
    if (!fp) {
        printf("[!] Error opening flag_main.txt\n");
        return;
    }
    char flag[128];
    if (fgets(flag, sizeof(flag), fp)) {
        printf("\n[+] ==================================================\n");
        printf("[+] [SUCCESS] Vault Breached! Main Flag:\n");
        printf("[+] %s", flag);
        printf("[+] ==================================================\n\n");
    }
    fclose(fp);
}

void secret_vault(uint64_t key1, uint64_t key2) {
    if (key1 == 0xdeadbeefcafebabeULL && key2 == 0x1337c0d31337c0d3ULL) {
        FILE *fp = fopen("/home/pwn/flag_hidden.txt", "r");
        if (!fp) {
            fp = fopen("flag_hidden.txt", "r");
        }
        if (!fp) {
            printf("[!] Error opening flag_hidden.txt\n");
            return;
        }
        char flag[128];
        if (fgets(flag, sizeof(flag), fp)) {
            printf("\n[+] ==================================================\n");
            printf("[+] [CONGRATULATIONS] Deep Vault Master Key Accepted!\n");
            printf("[+] Hidden Flag: %s", flag);
            printf("[+] ==================================================\n\n");
        }
        fclose(fp);
        exit(0);
    } else {
        printf("[-] [AUTH ERROR] Invalid secret vault credentials.\n");
        printf("    Key1: 0x%016" PRIx64 " (Expected: 0xdeadbeefcafebabe)\n", key1);
        printf("    Key2: 0x%016" PRIx64 " (Expected: 0x1337c0d31337c0d3)\n", key2);
        exit(1);
    }
}

// ROP gadget helper for calling convention setup
void gadget_hub() {
#if defined(__x86_64__) || defined(_M_X64)
    __asm__(
        "pop %rdi;\n"
        "pop %rsi;\n"
        "ret;\n"
    );
#endif
}

void challenge_1_variable_overwrite() {
    // Packed struct to ensure predictable stack offset
    struct __attribute__((packed)) {
        char user_buffer[64];
        volatile uint32_t auth_token;
        volatile uint32_t target_role;
    } session;

    session.auth_token = 0x11223344;
    session.target_role = 0x00000001; // Normal User

    printf("\n--- Mode 1: Stack Variable Control (Main Flag) ---\n");
    printf(" [*] Buffer Address      : %p\n", session.user_buffer);
    printf(" [*] Initial Target Role : 0x%08x (Required: 0x1337c0de)\n", session.target_role);
    printf(" [*] Initial Auth Token  : 0x%08x (Required: 0xdeadbeef)\n", session.auth_token);
    printf("Enter access payload > ");

    ssize_t n = read(0, session.user_buffer, 128);
    (void)n;

    printf(" [*] Result Target Role  : 0x%08x\n", session.target_role);
    printf(" [*] Result Auth Token   : 0x%08x\n", session.auth_token);

    if (session.auth_token == 0xdeadbeef && session.target_role == 0x1337c0de) {
        printf("[+] Variable constraints satisfied!\n");
        print_main_flag();
    } else {
        printf("[-] Access Denied. Stack variable integrity maintained.\n");
    }
}

void challenge_2_rop_diagnostic() {
    char diag_buf[64];
    memset(diag_buf, 0, sizeof(diag_buf));

    printf("\n--- Mode 2: Advanced Diagnostic Mode (Hidden Flag / ROP) ---\n");
    printf(" [*] Diagnostic Buffer Address : %p\n", diag_buf);
    printf(" [*] Secret Vault Function     : %p\n", secret_vault);
    printf(" [*] Gadget Hub (function base): %p  (find 'pop rdi; pop rsi; ret' inside via ROPgadget)\n", gadget_hub);
    printf("Enter diagnostic payload > ");

    ssize_t n = read(0, diag_buf, 256);
    (void)n;
    printf("[*] Diagnostic complete. Returning...\n");
}

int main(int argc, char *argv[]) {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    print_banner();

    while (1) {
        printf("\nAvailable Operations:\n");
        printf(" 1. Stack Variable Overwrite Auth (Main Flag)\n");
        printf(" 2. Diagnostic ROP Execution (Hidden Flag)\n");
        printf(" 3. Exit\n");
        printf("Choice > ");

        int choice = 0;
        if (scanf("%d", &choice) != 1) {
            break;
        }

        // Consume trailing newline
        int c;
        while ((c = getchar()) != '\n' && c != EOF);

        switch (choice) {
            case 1:
                challenge_1_variable_overwrite();
                break;
            case 2:
                challenge_2_rop_diagnostic();
                break;
            case 3:
                printf("[*] Goodbye!\n");
                return 0;
            default:
                printf("[!] Invalid option.\n");
                break;
        }
    }
    return 0;
}
