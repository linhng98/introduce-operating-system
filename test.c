#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void change_value_pointer(void *val)
{
    int *var = (int *)val;
    *var = -1;
}

int main()
{
    int a = 1;
    change_value_pointer(&a);
    printf("%d\n", a);
}