#include <stdio.h>
#include <string.h>

#define MAX_ARRAY_SIZE 64
char string[MAX_ARRAY_SIZE];

int main(int argc, char* argv[])
{
    memset(string, 0, sizeof(string));
    snprintf(string, MAX_ARRAY_SIZE, "Hello\n");
    return 0;
}
