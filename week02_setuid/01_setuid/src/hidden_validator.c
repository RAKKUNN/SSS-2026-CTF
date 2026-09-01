#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef __APPLE__
static inline int setresuid(uid_t r, uid_t e, uid_t s) {
    (void)s;
    return setreuid(r, e);
}
#endif

#define EXPECTED_ENV_KEY "SYSLAB_SETUID_VERIFICATION_2026"
#define EXPECTED_FD_TOKEN "0xCAFE_BABE_ROOT\n"

void print_hidden_flag() {
    FILE *fp = fopen("/root/flag_hidden.txt", "r");
    if (!fp) {
        fp = fopen("flag_hidden.txt", "r");
    }
    if (!fp) {
        printf("[!] Error: flag_hidden.txt not found or unreadable.\n");
        return;
    }
    char flag[128];
    if (fgets(flag, sizeof(flag), fp)) {
        printf("\n[+] ==================================================\n");
        printf("[+] [CONGRATULATIONS] Set-UID Hidden Flag Discovered!\n");
        printf("[+] %s", flag);
        printf("[+] ==================================================\n\n");
    }
    fclose(fp);
}

int main(int argc, char *argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0);

    printf("=====================================================\n");
    printf("         Set-UID Deep Verification Validator         \n");
    printf("=====================================================\n");

    // Condition 1: Check EUID (must have root privileges via SUID)
    if (geteuid() != 0) {
        printf("[-] [FAIL] Condition 1: Must be executed with root EUID (Set-UID missing).\n");
        return 1;
    }
    printf("[+] [PASS] Condition 1: Effective UID is root (0).\n");

    // Condition 2: Check Environment Variable
    const char *env_val = getenv("SECURE_KEY");
    if (!env_val || strcmp(env_val, EXPECTED_ENV_KEY) != 0) {
        printf("[-] [FAIL] Condition 2: Missing or invalid SECURE_KEY environment variable.\n");
        printf("    Hint: Pass SECURE_KEY='%s'\n", EXPECTED_ENV_KEY);
        return 2;
    }
    printf("[+] [PASS] Condition 2: SECURE_KEY environment variable verified.\n");

    // Condition 3: Check File Descriptor 3
    char fd_buf[64] = {0};
    ssize_t bytes_read = read(3, fd_buf, sizeof(fd_buf) - 1);
    if (bytes_read <= 0) {
        printf("[-] [FAIL] Condition 3: File descriptor 3 is not open or readable.\n");
        printf("    Hint: The calling process must setup an open pipe on FD 3 containing token.\n");
        return 3;
    }

    if (strncmp(fd_buf, EXPECTED_FD_TOKEN, strlen(EXPECTED_FD_TOKEN)) != 0) {
        printf("[-] [FAIL] Condition 3: FD 3 token mismatch. Received: %s\n", fd_buf);
        return 4;
    }
    printf("[+] [PASS] Condition 3: FD 3 authentication pipe token verified.\n");

    // Elevate privileges and reveal hidden flag
    setresuid(0, 0, 0);
    print_hidden_flag();
    return 0;
}
