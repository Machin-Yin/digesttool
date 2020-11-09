#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "dealcmdpara.h"

#define TOOL_VERSION "1.0.1"

int main(int argc, char *argv[])
{
    int ret = -1;
    int opt;
    char path[512] = {0};
    char file[512] = {0};
    char tmp[512] = {0};
    char *p = NULL;
    unsigned char digest_value[65] = {0};

    while ((opt = getopt(argc, argv, "p:f:hv")) != -1)
    {
        switch (opt)
        {
		case 'v':
			printf("version: %s\n", TOOL_VERSION);
			return 0;
        case 'h':
            printf("用法：\n");
            printf("    1: %s -v\n", argv[0]);
            printf("    2: %s -h\n", argv[0]);
            printf("    3: %s -f /home/test.so\n", argv[0]);
            printf("    4: %s -p /lib -f /home/filelist\n", argv[0]);
            printf("单参数：\n");
            printf("    -v	版本\n");
            printf("    -h	帮助\n");
            printf("    -f	后跟要计算摘要值文件的全路径\n");
            printf("双参数：\n");
            printf("    -p	后跟要扫描的路径\n");
            printf("    -f	后跟扫描结果存储文件全路径\n");
            return 0;
        case 'p':
            strcpy(path, optarg);
            break;
        case 'f':
            strcpy(file, optarg);
            break;
        default:
            printf("Command parameter error, use -h for help.\n");
            printf("eg: %s -h\n", argv[0]);
            return ret;
        }
    }

    if (path[0] && file[0])
    {
		if(path[0] != '/')
        {
            printf("Please use full path follow \"-p\"!\n");
            return -1;
        }

        strcpy(tmp, file);
        if (strstr(tmp, "/"))
        {
            printf("tmp: %s", tmp);
            p = strrchr(tmp, '/');
            if (p != tmp)
            {
                printf("p != tmp\n");
                *p = '\0';
                if (access(tmp, F_OK) != 0)
                {
                    printf("dir %s not exist\n", tmp);
                    return -1;
                }
            }
        }
        printf("扫描中……\n");
        get_filelist(path, file);
        printf("扫描结束！\n");
    }
    else if (file[0])
    {
        if (access(file, F_OK) != 0)
        {
            printf("file %s not exist\n", file);
            return -1;
        }

        ret = filesm3_64str(file, digest_value, 65);
        if (0 == ret)
        {
            // printf("file        : %s\n", file);
            printf("digest_value: %s\n", digest_value);
            return 0;
        }
    }
    else
    {
        printf("Command parameter error, use -h for help.\n");
        printf("eg: %s -h\n", argv[0]);
        ret = -1;
    }

    return ret;
}