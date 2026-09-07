#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    printf("=====================================================\n");
    printf("     [Set-UID Level 1: Intro] Privileged File Reader \n");
    printf("=====================================================\n");
    printf(" [*] Real UID      (getuid)  : %d\n", getuid());
    printf(" [*] Effective UID (geteuid) : %d\n", geteuid());
    printf("=====================================================\n");

    if (argc < 2) {
        printf("Usage: %s <filepath>\n", argv[0]);
        printf("Example: %s /etc/shadow\n", argv[0]);
        return 1;
    }

    printf("[*] Attempting to read: %s\n", argv[1]);
    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        perror("[-] fopen failed");
        return 1;
    }

    char buf[256];
    printf("\n--- File Contents ---\n");
    while (fgets(buf, sizeof(buf), fp)) {
        printf("%s", buf);
    }
    printf("\n--- End of File ---\n");
    fclose(fp);
    return 0;
}
