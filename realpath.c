#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

int main(int argc, char* argv[])
{
    int ret = 0;

#ifdef PATH_MAX
    static int const sc_path_max = PATH_MAX;
#else
    static int const sc_path_max = pathconf(path, _PC_PATH_MAX);
    if (sc_path_max <= 0) {
        sc_path_max = 4096;
    }
#endif

    if (1 < argc) {
        char* p_realpath = NULL;

        p_realpath = (char*)malloc(sc_path_max);
        ret = (NULL == realpath(argv[1], p_realpath)) ? (-1) : 0;
        if (-1 == ret) {
            printf("[ERROR] get realpath failed.\n");
        } else {
            printf("%s\n", p_realpath);
        }
        free(p_realpath);
    } else {
        printf("usage: realpath pathname\n");
    }

    return ret;
}
