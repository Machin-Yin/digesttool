#ifndef __DEALCMDPARA_H_
#define __DEALCMDPARA_H_

int filesm3_64str(const char *path,
                  char *dig_str, int dig_len);
int get_filelist(const char *dirname, char *filepath);
int elf_check(const char *filename);
int is_link(char *path);
int save_filelist(char *content, char *filename);
int file_kind(char *filename);
int check_shell(const char *filename);
int check_python(const char *filename);
int is_begin_with(const char *str1, char *str2);
int is_end_with(const char *str1, char *str2);

#endif
