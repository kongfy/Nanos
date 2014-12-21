#include "common.h"
#include "device.h"

static int print_ctrl(const char *, void *, int *);

// 返回打印出的字符数量
int printk(char** formatp)
{
    void *arg_base = formatp;
    char *format = *formatp;

    int i = 0;
    int arg_count = 0;
    int char_count = 0;
    while (format[i]) {
        if ('%' == format[i]) {
            if ('%' == format[i + 1]) {
                // "%%"打印出一个'%'字符
                i++;
            } else {
                void *arg = NULL;
                int ctrl_len = 0;

                arg_count++;
                arg = arg_base + (4 * arg_count);
                char_count += print_ctrl(&format[i], arg, &ctrl_len);
                i += ctrl_len;
                continue;
            }

        }

        putchar(format[i]);
        char_count++;
        i++;
    }

    return char_count;
}

// 对单个参数进行打印，返回打印的字符数量
static int print_ctrl(const char *ctrl, void *arg, int *ctrl_len)
{
    if ('%' != ctrl[0]) {
        *ctrl_len = 0;
        return 0;
    }

    int char_count = 0;
    // 暂时只支持单个控制字符
    char ctrl_char = ctrl[1];

    switch (ctrl_char) {
    case 'd': {
        int n = *(int *)arg;
        char temp[20];
        int len = 0;

        if (n < 0) {
            n = -n;
            putchar('-');
            char_count++;
        }

        while (n != 0) {
            temp[len++] = n % 10;
            n /= 10;
        }

        if (0 == len) {
            putchar('0');
            char_count++;
        } else {
            int i;
            for (i = len - 1; i >= 0; --i) {
                putchar('0' + temp[i]);
            }
            char_count += len;
        }
        break;
    }
    case 'x': {
        unsigned int n = *(unsigned int *)arg;
        char temp[20];
        int len = 0;

        while (n != 0) {
            temp[len++] = n % 16;
            n /= 16;
        }

        putchar('0');
        putchar('x');
        char_count += 2;
        if (0 == len) {
            putchar('0');
            char_count++;
        } else {
            int i;
            for (i = len - 1; i >= 0; --i) {
                if (temp[i] < 10) {
                    putchar('0' + temp[i]);
                } else {
                    putchar('a' + temp[i] - 10);
                }
            }
            char_count += len;
        }

        break;
    }
    case 's': {
        char *string = *(char **)arg;
        int i = 0;

        while (string[i]) {
            putchar(string[i]);
            char_count++;
            i++;
        }
        break;
    }
    case 'c': {
        putchar(*(char *)arg);
        char_count++;
        break;
    }
    default:
        assert(0);
        break;
    }

    *ctrl_len = 2;
    return char_count;
}
