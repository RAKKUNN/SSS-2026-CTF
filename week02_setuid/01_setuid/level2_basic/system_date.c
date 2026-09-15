#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    printf("=====================================================\n");
    printf("     [Set-UID Level 2: Basic] Server Time Utility    \n");
    printf("=====================================================\n");
    
    // Elevate real uid to effective uid to prevent dash privilege drop
    setreuid(geteuid(), geteuid());

    printf("[*] Executing system time diagnostic utility...\n");
    
    // Vulnerability: Calling 'date' via relative path
    system("date");

    return 0;
}
