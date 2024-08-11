#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    char buffer[256];
    FILE *fp;

    printf("Starting packet sniffer display...\n");

    while (1) {
        fp = popen("dmesg | grep 'Sniffer'", "r");
        if (fp == NULL) {
            printf("Failed to run command\n");
            exit(1);
        }

        while (fgets(buffer, sizeof(buffer), fp) != NULL) {
            printf("%s", buffer);
        }

        pclose(fp);
        sleep(1);
    }

    return 0;
}

