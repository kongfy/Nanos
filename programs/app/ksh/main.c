#include "stdio.h"
#include "unistd.h"
#include "string.h"
#include "const.h"
#include "stat.h"
#include "ksh.h"

static char logo[] = {
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x5f, 0x20, 0x20, 0x5f, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x5f, 0x5f,
  0x5f, 0x20, 0x20, 0x5f, 0x5f, 0x5f, 0x20, 0x20, 0x20, 0x5f, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x5f, 0x20, 0x20,
  0x5f, 0x5f, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x5f, 0x5f, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7c, 0x20, 0x5c, 0x7c,
  0x20, 0x7c, 0x5f, 0x5f, 0x20, 0x5f, 0x20, 0x5f, 0x20, 0x5f, 0x20, 0x2f,
  0x20, 0x5f, 0x20, 0x5c, 0x2f, 0x20, 0x5f, 0x5f, 0x7c, 0x20, 0x7c, 0x20,
  0x7c, 0x5f, 0x5f, 0x20, 0x5f, 0x20, 0x20, 0x5f, 0x20, 0x20, 0x7c, 0x20,
  0x7c, 0x2f, 0x20, 0x2f, 0x5f, 0x5f, 0x5f, 0x20, 0x5f, 0x20, 0x5f, 0x20,
  0x20, 0x5f, 0x5f, 0x20, 0x5f, 0x20, 0x2f, 0x20, 0x5f, 0x7c, 0x5f, 0x20,
  0x20, 0x5f, 0x20, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7c, 0x20,
  0x2e, 0x60, 0x20, 0x2f, 0x20, 0x5f, 0x60, 0x20, 0x7c, 0x20, 0x27, 0x20,
  0x5c, 0x20, 0x28, 0x5f, 0x29, 0x20, 0x5c, 0x5f, 0x5f, 0x20, 0x5c, 0x20,
  0x7c, 0x20, 0x27, 0x5f, 0x20, 0x5c, 0x20, 0x7c, 0x7c, 0x20, 0x7c, 0x20,
  0x7c, 0x20, 0x27, 0x20, 0x3c, 0x2f, 0x20, 0x5f, 0x20, 0x5c, 0x20, 0x27,
  0x20, 0x5c, 0x2f, 0x20, 0x5f, 0x60, 0x20, 0x7c, 0x20, 0x20, 0x5f, 0x7c,
  0x20, 0x7c, 0x7c, 0x20, 0x7c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x7c, 0x5f, 0x7c, 0x5c, 0x5f, 0x5c, 0x5f, 0x5f, 0x2c, 0x5f, 0x7c, 0x5f,
  0x7c, 0x7c, 0x5f, 0x5c, 0x5f, 0x5f, 0x5f, 0x2f, 0x7c, 0x5f, 0x5f, 0x5f,
  0x2f, 0x20, 0x7c, 0x5f, 0x2e, 0x5f, 0x5f, 0x2f, 0x5c, 0x5f, 0x2c, 0x20,
  0x7c, 0x20, 0x7c, 0x5f, 0x7c, 0x5c, 0x5f, 0x5c, 0x5f, 0x5f, 0x5f, 0x2f,
  0x5f, 0x7c, 0x7c, 0x5f, 0x5c, 0x5f, 0x5f, 0x2c, 0x20, 0x7c, 0x5f, 0x7c,
  0x20, 0x20, 0x5c, 0x5f, 0x2c, 0x20, 0x7c, 0x0a, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7c, 0x5f,
  0x5f, 0x2f, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7c, 0x5f, 0x5f, 0x5f, 0x2f, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x7c, 0x5f, 0x5f, 0x2f, 0x20, 0x0a, 0x00
};

#define MAX_LEN 256
#define MAXARG  16
uint8_t buf[MAX_LEN];
uint8_t path[MAX_LEN];

// return argc
int parse(uint8_t *buf, void *argv, char **redirect_in, char **redirect_out)
{
    int argc = 0;
    char *p = (char *)buf;
    char **t = (char **)argv;

    while (is_blank_char(*p) && *p != '\0') *(p++) = '\0';
    *(t++) = p;

    char *q = p;
    while (!is_blank_char(*q) && *q != '\0') q++;

    if (p != q) {
        argc = 1;
    } else {
        return 0;
    }

    bool flag = false;
    bool f_in = false;
    bool f_out = false;
    while (*q != '\0') {
        if (!is_blank_char(*q)) {
            if ('<' == *q) {
                f_in = true;
                *q = '\0';
                flag = false;
            } else if ('>' == *q) {
                f_out = true;
                *q = '\0';
                flag = false;
            } else if (!flag) {
                if (!f_in && !f_out) {
                    *(t++) = q;
                    argc++;
                } else {
                    if (f_in) {
                        *redirect_in = q;
                        f_in = false;
                    }

                    if (f_out) {
                        *redirect_out = q;
                        f_out = false;
                    }
                }
                flag = true;
            }
        } else {
            *q = '\0';
            flag = false;
        }
        q++;
    }

    return argc;
}

char *path_extend(char *cmd)
{
    int len = strlen(cmd);
    if (len == 0) {
        return cmd;
    }

    if (len >= 1) {
        if (cmd[0] == '/') {
            return cmd;
        }
    }

    if (len >= 2) {
        if (cmd[0] == '.' && cmd[1] == '/') {
            return cmd;
        }
    }

    strcpy((char *)path, "/bin/");
    strcpy((char *)&path[5], cmd);
    return (char *)path;
}

bool buildin(int argc,  char *argv[]);

static struct stat stat_buf;

int main(int argc, char *argv[])
{
    printf(logo);

    while (true) {
        printf("# ");

        int nread = read(STDIN_FILENO, buf, 255);
        buf[nread] = 0;

        char *argv[MAXARG];
        int i;
        for (i = 0; i < MAXARG; ++i) argv[i] = NULL;

        char *redirect_in = NULL, *redirect_out = NULL;
        int argc = parse(buf, argv, &redirect_in, &redirect_out);

        if (!argc) continue;
        char *cmd = argv[0];

        // check build-in first
        if (buildin(argc, argv)) {
            continue;
        }

        cmd = path_extend(cmd);

        pid_t pid = fork();
        if (pid == 0) {
            if (stat(cmd, &stat_buf)) {
                printf("-ksh: %s: No such file or directory\n", cmd);
                return 1;
            }

            if (DIRECTORY == stat_buf.type) {
                printf("-ksh: %s: Is a directory\n", cmd);
                return 1;
            }

            // redirect standard input and output
            if (redirect_in) {
                if (stat(redirect_in, &stat_buf)) {
                    printf("-ksh: %s: No such file or directory\n", redirect_in);
                    return 1;
                }

                if (DIRECTORY == stat_buf.type) {
                    printf("-ksh: %s: Is a directory\n", redirect_in);
                    return 1;
                }

                close(STDIN_FILENO);
                int fd = open(redirect_in);
                lseek(fd, 0, SEEK_SET);
            }

            if (redirect_out) {
                if (stat(redirect_out, &stat_buf)) {
                    int fd = open(redirect_out);
                    if (fd < 0) {
                        if (-2 == fd) {
                            printf("-ksh %s: No such file or directory\n", redirect_out);
                        } else if (-3 == fd) {
                            printf("-ksh %s: Invalid file name\n", redirect_out);
                        }
                    } else {
                        close(fd);
                    }
                }

                if (DIRECTORY == stat_buf.type) {
                    printf("ksh: %s: Is a directory\n", redirect_out);
                    return 1;
                }

                close(STDOUT_FILENO);
                int fd = open(redirect_out);
                lseek(fd, 0, SEEK_END);
            }

            int err = exec(cmd, argv);
            if (err == -500) {
                printf("-ksh: %s: Can not be excuted\n", cmd);
                return 1;
            }
        } else {
            waitpid(pid);
        }
    }

    return 0;
}
