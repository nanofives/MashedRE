#include <stdio.h>
#include <time.h>
int main(void) {
    __int64 v[] = {-1, 0, 86399, 32535215999i64, 32535215999i64 + 1, 32535244799i64, 32535244800i64, 0x100000000000i64 - 1, 0x100000000000i64, 1100000000};
    int i; char *s;
    for (i = 0; i < (int)(sizeof(v) / sizeof(v[0])); ++i) {
        s = _ctime64(&v[i]);
        printf("%I64d -> %s", v[i], s ? s : "NUi64\n");
    }
    return 0;
}
