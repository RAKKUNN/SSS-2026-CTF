#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

#ifdef __APPLE__
static inline int setresuid(uid_t r, uid_t e, uid_t s) {
    (void)s;
    return setreuid(r, e);
}
#endif

void print_banner() {
    printf("=====================================================\n");
    printf("     Secure System Logger & Diagnostics Daemon       \n");
    printf("=====================================================\n");
    printf(" [*] Current Real UID : %d\n", getuid());
    printf(" [*] Current Eff  UID : %d\n", geteuid());
    printf("=====================================================\n");
}

void view_logs() {
    FILE *fp = fopen("/var/log/sysdiag.log", "r");
    if (!fp) {
        printf("[!] Error: Cannot open /var/log/sysdiag.log\n");
        return;
    }
    char line[256];
    printf("\n--- System Diagnostics Log ---\n");
    while (fgets(line, sizeof(line), fp)) {
        printf("%s", line);
    }
    fclose(fp);
    printf("--- End of Log ---\n\n");
}

void run_maintenance() {
    printf("\n[*] Initiating automated log maintenance...\n");
    printf("[*] Synchronizing privilege state for maintenance task...\n");
    
    // Set real, effective, and saved UIDs to effective UID (root: 0)
    // to preserve privileged execution under modern shells (/bin/dash)
    if (setresuid(geteuid(), geteuid(), geteuid()) != 0) {
        perror("setresuid");
        return;
    }

    printf("[+] Privileges synchronized (UID=%d, EUID=%d).\n", getuid(), geteuid());
    printf("[*] Executing: clean_temp_logs /var/log/sysdiag.log\n");

    // Vulnerability: Relative command invocation without path sanitization
    int res = system("clean_temp_logs /var/log/sysdiag.log");
    if (res == -1) {
        perror("system");
    } else {
        printf("[+] Maintenance completed successfully.\n\n");
    }
}

void show_uptime() {
    printf("\n--- Host Uptime ---\n");
    system("/usr/bin/uptime");
    printf("\n");
}

int main(int argc, char *argv[]) {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    print_banner();

    while (1) {
        printf("1. View System Diagnostics Log\n");
        printf("2. Run Log Maintenance & Rotation\n");
        printf("3. System Uptime\n");
        printf("4. Exit\n");
        printf("Choice > ");

        int choice = 0;
        if (scanf("%d", &choice) != 1) {
            break;
        }

        switch (choice) {
            case 1:
                view_logs();
                break;
            case 2:
                run_maintenance();
                break;
            case 3:
                show_uptime();
                break;
            case 4:
                printf("[*] Terminating logger session. Bye!\n");
                return 0;
            default:
                printf("[!] Invalid choice.\n");
                break;
        }
    }
    return 0;
}
