#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "sm3.h"
#include "dealcmdpara.h"

#define TCA_SM3_DIGEST_MAX_SIZE 32

char *filter_list[] = {
	"/dev",
	"/proc",
	"/mnt",
	"/boot",
	"/tmp",
	"/var",
	"/run",
	"/sys",
	"/lost+found",
	"/media",
	NULL};

int get_filelist(const char *dirname, char *filepath)
{
	printf("get_filelist\n");
	printf("dirname: %s\n", dirname);
	int ret = -1;
	int is_filter = -1;
	int i = 0;

	if (dirname == NULL || filepath == NULL)
	{
		printf("get_filelist param error!");
		return ret;
	}

	char tmp_path[512] = {0};
	char path[512] = {0};
	char dstpath[512] = {0};
	char *substr = NULL;
	struct dirent *filename;
	DIR *dir;
	unsigned char digest_value[65] = {0};

	dir = opendir(dirname);
	if (dir == NULL)
	{
		printf("open dir %s error!\n", dirname);
		return ret;
	}

	while ((filename = readdir(dir)) != NULL)
	{
		if (!strcmp(filename->d_name, ".") || !strcmp(filename->d_name, ".."))
			continue;

		if (!strcmp(dirname, "/"))
		{
			while (filter_list[i] != NULL)
			{
				memset(tmp_path, 0, 512);
				sprintf(tmp_path, "%s%s", dirname, filename->d_name);
				if (!strcmp(tmp_path, filter_list[i]))
				{
					// printf("is_filter filename->d_name: %s\n", filename->d_name);
					is_filter = 0;
					break;
				}
				i++;
			}
			i = 0;
		}
		if (is_filter == 0)
		{
			is_filter = -1;
			continue;
		}

		substr = strrchr(dirname, '/');
		if (strcmp(substr, "/"))
		{
			sprintf(path, "%s/%s", dirname, filename->d_name);
		}
		else
		{
			sprintf(path, "%s%s", dirname, filename->d_name);
		}
		printf("path: %s\n", path);

		struct stat s;
		lstat(path, &s);

		if (S_ISDIR(s.st_mode))
		{
			get_filelist(path, filepath);
		}
		else
		{
			if (is_link(path))
			{
				continue;
			}
			ret = file_kind(path);

			if (ret == 1 || ret == 2 || ret == 3)
			{
				char line[512] = {0};
				ret = filesm3_64str(path, digest_value, 65);
				if (ret == 0)
				{
					strcpy(line, digest_value);
					strcat(line, " ");
					strcat(line, path);
					save_filelist(line, filepath);
				}
			}
		}
	}
	closedir(dir);
	return 0;
}

int filesm3_64str(const char *path,
				  char *dig_str, int dig_len)
{
	int ret = -1, n;
	unsigned char
		dig_val[TCA_SM3_DIGEST_MAX_SIZE] = {0x00};
	unsigned char tmp_buf[16] = {0x00};

	if (NULL == dig_str || dig_len < 64)
	{
		return ret;
	}

	memset(dig_str, 0x00, dig_len);

	if (sm3_file(path, dig_val))
	{
		return ret;
	}

	for (n = 0; n < TCA_SM3_DIGEST_MAX_SIZE; n++)
	{
		memset(tmp_buf, 0x00, 16);
		snprintf((char *)tmp_buf, 16, "%02X", dig_val[n]);
		strcat(dig_str, tmp_buf);
	}
	ret = 0;

	return ret;
}

int elf_check(const char *filename)
{
	int ret = 0;
	FILE *fp = NULL;
	unsigned char data[8] = {0};

	if (NULL == filename)
	{
		return -1;
	}

	fp = fopen(filename, "r");
	if (NULL == fp)
	{
		return -1;
	}

	ret = fread(data, sizeof(char), sizeof(data), fp);

	if (sizeof(data) == ret)
	{
		if (data[0] == 0x7f && data[1] == 0x45 && data[2] == 0x4c && data[3] == 0x46)
		{

			fclose(fp);
			fp = NULL;

			return 0;
		}
	}

	fclose(fp);
	fp = NULL;

	return -1;
}

int is_link(char *path)
{
	int ret = -1;

	if (path == NULL)
	{
		goto err_out;
	}
	char buf[1024] = {0};

	readlink(path, buf, 1024);

	ret = strlen(buf);
	return ret;

err_out:
	return ret;
}

int save_filelist(char *content, char *filename)
{
	// printf("content: %s\n", content);
	// printf("filename: %s\n", filename);

	FILE *fp = NULL;

	fp = fopen(filename, "a+");
	if (!fp)
	{
		printf("open %s failed\n", filename);
		return -1;
	}
	fputs(content, fp);
	fputs("\n", fp);

	fclose(fp);
	return 0;
}

static int endwith(char *src, char *str)
{
	char *p;

	p = strrchr(src, '.');

	if (p != NULL && strcmp(p, str) == 0)
	{
		//	printf("filename [%s:%s]\n",src, p);
		return 0;
	}
	else
		return -1;
}

/*
 * return 
 * 0:normal
 * 1:shell
 * 2:ko
 * 3:elf
 */
int file_kind(char *filename)
{
	printf("file_kind filename: %s\n", filename);
	int ret = 0;
	int fd;

	if (!endwith(filename, ".ko"))
	{
		ret = 2;
		printf(".ko ret: %d\n\n", ret);
		return ret;
	}

	if (!endwith(filename, ".o"))
	{
		ret = 0;
		printf("file_kind ret: %d\n\n", ret);
		return ret;
	}

	if (check_shell(filename) == 0)
	{
		ret = 1;
		printf("check_shell ret: %d\n\n", ret);
		return ret;
	}

	if (check_python(filename) == 0)
	{
		ret = 1;
		printf("check_python ret: %d\n\n", ret);
		return ret;
	}

	if (elf_check(filename) == 0)
	{
		ret = 3;
		printf("elf_check ret: %d\n\n", ret);
	}
	else
	{
		ret = 0;
		printf("elf_check ret: %d\n\n", ret);
	}

	return ret;
}

int check_shell(const char *filename)
{
    FILE *fp;
    char buf[30];
    char tmp[30];
    if ((fp = fopen(filename, "r")) == NULL)
    {
        return -1;
    }
    fgets(buf, 30, fp);
    fclose(fp);
    int i;
    for (i = 0; buf[i]; i++)
    {
        if (buf[i] == '\n')
        {
            buf[i] = '\0';
        }
    }

    strncpy(tmp, buf+2, 30 - 2);

    if(is_begin_with(tmp, "/bin/bash") == 0 \
       || is_begin_with(tmp, "/bin/nawk") == 0 \
       || is_begin_with(tmp, "/bin/zsh") == 0 \
       || is_begin_with(tmp, "/bin/sh") == 0 \
       || is_begin_with(tmp, "/bin/ksh") == 0 \
       || is_begin_with(tmp, "/bin/dash") == 0 \
       || is_begin_with(tmp, "/bin/env sh") == 0 \
       || is_begin_with(tmp, "/bin/env bash") == 0 \
       || is_begin_with(tmp, "/bin/env zsh") == 0 \
       || is_begin_with(tmp, "/bin/env ksh") == 0)
    {
        printf("%s is shell\n", filename);
        return 0; // is shell file
    }
    else
    {
        printf("%s not shell\n", filename);
        return 1; // not shell file
    }
}

int check_python(const char *filename)
{
    FILE *fp;
    char buf[50];
    char tmp[50];
    if ((fp = fopen(filename, "r")) == NULL)
    {
        return -1;
    }
    fgets(buf, 50, fp);
    fclose(fp);
    int i;
    for (i = 0; buf[i]; i++)
    {
        if (buf[i] == '\n')
        {
            buf[i] = '\0';
        }
    }

    strncpy(tmp, buf+2, 50 - 2);
	
    if(is_begin_with(buf, "#!/bin/python") == 0 \
       || is_begin_with(buf, "#! /bin/python") == 0 \
       || is_begin_with(buf, "#!/usr/bin/python") == 0 \
       || is_begin_with(buf, "#! /usr/bin/python") == 0 \
       || is_begin_with(buf, "#!/usr/local/bin/python") == 0 \
       || is_begin_with(buf, "#! /usr/local/bin/python") == 0 \
       || is_begin_with(buf, "eval \"exec /bin/python") == 0 \
       || is_begin_with(buf, "eval \"exec /usr/bin/python") == 0 \
       || is_begin_with(buf, "eval \"exec /usr/local/bin/python") == 0 \
       || is_begin_with(tmp, "/bin/env python") == 0 \
	   || is_end_with(filename, ".pyc") == 0 \
	   || is_end_with(filename, ".pyo") == 0)
    {
		printf("buf: %s\n", buf);
        printf("%s is python\n", filename);
        return 0; // is python file
    }
    else
    {
		printf("buf: %s\n", buf);
        printf("%s not python\n", filename);
        return 1; // not python file
    }
}

/**判断str1是否以str2开头
 * 如果是返回0
 * 不是返回1
 * 出错返回-1
 * */
int is_begin_with(const char *str1, char *str2)
{
    if(str1 == NULL || str2 == NULL)
      return -1;
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    if((len1 < len2) || (len1 == 0 || len2 == 0))
      return -1;
    char *p = str2;
    int i = 0;
    while(*p != '\0')
    {
      if(*p != str1[i])
        return 1;
      p++;
      i++;
    }
    return 0;
}

/**判断str1是否以str2结尾
 * 如果是返回0
 * 不是返回1
 * 出错返回-1
 * */
int is_end_with(const char *str1, char *str2)
{
    if(str1 == NULL || str2 == NULL)
        return -1;
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    if((len1 < len2) ||  (len1 == 0 || len2 == 0))
        return -1;
    while(len2 >= 1)
    {
        if(str2[len2 - 1] != str1[len1 - 1])
            return 1;
        len2--;
        len1--;
    }
    return 0;
}

#if 0
int shell_X(const char *filename)
{
	/*判断是否为脚本文件用fget函数*/

	char buf[20];
	FILE *fp;
	int n = 0; //标注是否是脚本文件，1是，0否
	int i, j = 0;

	if ((fp = fopen(filename, "r")) == NULL)
	{
		return -1;
	}

	fgets(buf, 20, fp);

	/*去除字符串中的空格*/
	for (i = 0; i < strlen(buf); i++)
	{
		if (buf[i] != ' ')
		{
			buf[j] = buf[i];
			j++;
		}
		if (j > 6)
			break;
	}
	buf[j] = '\0';
	if (strcmp(buf, "#!/bin/") == 0)
	{
		n = 1;
	}

	fclose(fp);
	if (n)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int python_X(const char *filename)
{
	FILE *fp;
	char buf[20];
	if ((fp = fopen(filename, "r")) == NULL)
	{
		return -1;
	}
	fgets(buf, 20, fp);
	fclose(fp);
	int i, j;
	for (i = 0, j = 0; buf[i]; i++)
	{
		if (buf[i] == '\n')
		{
			buf[i] = '\0';
		}
		if (buf[i] != ' ')
		{
			buf[j++] = buf[i];
		}
	}
	buf[j] = '\0';
	if (strstr(filename, ".py") != NULL || (strstr(buf, "python") != NULL && strstr(buf, "#!/") != NULL))
	{
		return 1; //是python文件
	}
	else
	{
		return 0; //不是python文件
	}
}
#endif