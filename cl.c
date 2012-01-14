#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

/* 布尔型 */
typedef char        bool;
#define FALSE       0
#define TRUE        (!FALSE)

/* strlen_e7替代strlen函数 */
static int strlen_e7(char const* pc_str)
{
    static int const MAX_LENGTH = 4096;

    int ret = 0;

    if (NULL == pc_str) {
        ret = -1;
    } else {
        int i = 0;

        for (i = 0; ('\0' != pc_str[i]) && (i < MAX_LENGTH); ++i);
        ret = i;
    }

    return ret;
}

/* 默认目录权限 */
#define DEFAULT_DIR_PMS         (S_IRUSR | S_IWUSR | S_IXUSR | \
                                 S_IRGRP | S_IXGRP | \
                                 S_IROTH | S_IXOTH)
/*#define DEFAULT_DIR_PMS         (0777)*/

/* 命令行参数 */
#define CMD_OPTIONS         ("has:d:")
static char const* spc_options = CMD_OPTIONS;

/* 选项结构 */
typedef struct _SOptions {
    bool m_usage;
    bool m_all;
    bool m_src;
    bool m_dest;
    char const* mpc_src;
    char const* mpc_dest;
} SOptions;

/* 错误类型 */
typedef enum {
    LACK_OF_ESSENTIAL_OPTIONS = 1,  /* 必要的参数缺失 */
    INVALID_PATH, /* 无效路径 */
    CD_FAILURE, /* 进入目录失败 */
    MKDIR_FAILURE, /* 创建目录失败 */
} EErrorMsgType;

/* 根据错误类型打印错误信息 */
static int ErrorMsg(int flag)
{
    switch (flag) {
    case LACK_OF_ESSENTIAL_OPTIONS:
        printf("[ERROR(%d)] no essential options or arguments provided!\n",
               errno);
        break;
    case INVALID_PATH:
        printf("[ERROR(%d)] invalid path!\n", errno);
        break;
    case CD_FAILURE:
        printf("[ERROR(%d)] cd destination directory failed.\n", errno);
        break;
    case MKDIR_FAILURE:
        printf("[ERROR(%d)] create directory failed.\n"
               "            maybe you don't have permission or "
               "the directory already exsited.\n", errno);
        break;
    default:
        printf("[ERROR(%d)] unknown error occured!\n", errno);
        break;
    }

    return -1;
}

/* "-h"选项处理函数 */
static void HandleOptionH(void)
{
    printf("usage: cl [-a] -s source [-d destination]\t"
           "create a soft-linked mirror for source directory.\n");
    printf("       cl -h\t\t\t\t\t"
           "show this message.\n");
}

/* 取得绝对路径 */
static int GetAbsltPath(char const* pc_path_orgn,
                        char* p_buf, size_t buf_size)
{
    int ret = 0;

    do {
        char const* pc_abstl_path = NULL;

        if (NULL == p_path_orgn) {
            ret = -1;
            break;
        }
        if ((NULL == p_buf) || (buf_size < PATH_MAX)) {
            ret = -1;
            break;
        }

    } while (0);

    return ret;
}

/* "-s"选项处理函数 */
static int HandleOptionS(bool all, char const* p_src, char const* p_dest)
{
    int ret = 0;
    DIR* p_dir_src = NULL;
    DIR* p_dir_dest = NULL;
    char* p_path_src = NULL;
    char* p_path_dest = NULL;

    /* 参数检查 */
    assert(NULL != p_src);
    assert(NULL != p_dest);

    /* 保存源目录及目标目录信息 */
    p_path_src = (char*)malloc(PATH_MAX);
    memset(p_path_src, 0, sizeof(p_path_src));
    if ((1 == strlen_e7(p_src)) && (0 == strncmp(".", p_src, 1))) {
        printf("[INFO] source dir saved: %s\n", getcwd(p_path_src, PATH_MAX));
    } else if ((2 == strlen_e7(p_src)) && (0 == strncmp("..", p_src, 2))) {
        char* p_last_sprtor = strrchr(getcwd(p_path_src, PATH_MAX), '/');

        memset(p_last_sprtor, 0, p_last_sprtor - p_path_src);
        printf("[INFO] source dir saved: %s\n", p_path_src);
    } else {
        printf("[INFO] source dir saved: %s\n",
               strncpy(p_path_src, p_src, PATH_MAX));
    }

    p_path_dest = (char*)malloc(PATH_MAX);
    memset(p_path_dest, 0, sizeof(p_path_dest));
    if ((1 == strlen_e7(p_dest)) && (0 == strncmp(".", p_dest, 1))) {
        printf("[INFO] destination dir saved: %s\n",
               getcwd(p_path_dest, PATH_MAX));
    } else if ((2 == strlen_e7(p_dest)) && (0 == strncmp("..", p_dest, 2))) {
        char* p_last_sprtor = strrchr(getcwd(p_path_dest, PATH_MAX), '/');

        memset(p_last_sprtor, 0, p_last_sprtor - p_path_dest);
        printf("[INFO] destination dir saved: %s\n", p_path_dest);
    } else {
        printf("[INFO] destination dir saved: %s\n",
               strncpy(p_path_dest, p_dest, PATH_MAX));
    }

    /********** 生成镜像 **********/
    p_dir_src = opendir(p_path_src);
    p_dir_dest = opendir(p_path_dest);
    do {
        struct dirent* p_dirent_src = NULL;

        /* 检测目录的有效性 */
        if ((NULL == p_dir_src) || (NULL == p_dir_dest)) {
            ret = ErrorMsg(INVALID_PATH);
            break;
        }

        /* 建立镜像目录 */
        ret = chdir(p_path_dest); /* 进入目标目录 */
        system("pwd");
        if (-1 == ret) {
            ret = ErrorMsg(CD_FAILURE);
            break;
        }
        printf("%s\n", p_path_src);
        printf("%s\n", &(strrchr(p_path_src, '/')[1]));
        ret = mkdir(&(strrchr(p_path_src, '/')[1]), DEFAULT_DIR_PMS); /* 创建镜 */
                                                                    /* 像目录 */
        if (-1 == ret) {
            ret = ErrorMsg(MKDIR_FAILURE);
            break;
        }

        /* 遍历目录 */
        while (NULL != (p_dirent_src = readdir(p_dir_src))) {
            printf("%s\n", p_dirent_src->d_name);
        }
    } while (0);
    closedir(p_dir_dest);
    closedir(p_dir_src);

    free(p_path_dest);
    free(p_path_src);
    return ret;
}

/* 选项及参数处理函数 */
static int HandleAll(SOptions const* pc_options)
{
    int ret = 0;

    do {
        /* 参数有效性检查 */
        if (NULL == pc_options) {
            ret = -1;
            break;
        }

        /* 必须的选项缺失 */
        if (!(pc_options->m_usage | pc_options->m_src)) {
            ret = ErrorMsg(LACK_OF_ESSENTIAL_OPTIONS);
            HandleOptionH();
            break;
        }
        
        /* "-h"选项处理 */
        if (TRUE == pc_options->m_usage) {
            HandleOptionH();
        }

        /* "-s"选项处理 */
        if (TRUE == pc_options->m_src) {
            if (NULL == pc_options->mpc_src) { /* 非法路径 */
                ret = -1;
                break;
            }
            ret = HandleOptionS(pc_options->m_all,
                                pc_options->mpc_src,
                                (NULL == pc_options->mpc_dest) ?
                                    "." : pc_options->mpc_dest);
        }
    } while (0);

    return ret;
}

int main(int argc, char* argv[])
{
    int ret = 0;
    SOptions opt = {
        .m_usage = FALSE,
        .m_all = FALSE,
        .m_src = FALSE,
        .m_dest = FALSE,
        .mpc_src = NULL,
        .mpc_dest = NULL,
    };

#if 0
    /* test */
    assert(-1 == strlen_e7(NULL));
    assert(0 == strlen_e7(""));
    assert(1 == strlen_e7("."));
    assert(2 == strlen_e7(".."));
#endif
    do {
        ret = getopt(argc, argv, spc_options);
        
        switch (ret) {
        case 'a':
            opt.m_all = TRUE;
            break;
        case 's':
            opt.m_src = TRUE;
            opt.mpc_src = optarg;
            printf("[INFO] source path specified: %s\n", opt.mpc_src);
            break;
        case 'd':
            opt.m_dest = TRUE;
            opt.mpc_dest = optarg;
            printf("[INFO] destinatin path specified: %s\n", opt.mpc_dest);
            break;
        case 'h':
            opt.m_usage = TRUE;
            break;
        default:
            break;
        };
    } while (-1 != ret);

    ret = HandleAll(&opt);

    return ret;
}
