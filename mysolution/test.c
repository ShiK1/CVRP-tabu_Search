#include <stdio.h>
#include <string.h>

int a[3] = {1, 2, 3};
int b[3];
int main(int argc, char const *argv[])
{
    for (int i = 0; i < 3; i++)
    {
        printf("%d", b[i]);
    }
    memcpy(b, a, sizeof(a));
    for (int i = 0; i < 3; i++)
    {
        printf("%d", b[i]);
    }

    return 0;
}