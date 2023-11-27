#include <algorithm>
#include <cerrno>
#include <cstdlib>
#include <fmt/core.h>
#include <fmt/format.h>
#include <string.h>
#include <unistd.h>
#include <vector>

/* [ file name ] : msd
 * [ file purpose ] : a wrapper around mount
 * [ mode ] : stable
 * [ sample ] : msd /mnt /sda2
 * [ sample2 ] : msd -lsblk
 */

char command[120];
char temp[100]; // input puts into command

void mountsd(char *str1, char *str2, char *exe_name)
{
    snprintf(command, 119, "sudo mount -o umask=000 /dev/");
    if (*(str1 + 0) == '/' && *(str2 + 0) != '/')
    {
        snprintf(temp, 99, "%s %s", str2, str1);
    }
    else if (*(str1 + 0) != '/' && *(str2 + 0) == '/')
    {
        snprintf(temp, 99, "%s %s", str1, str2);
    }
    else
    {
        fprintf(stderr,
                "%s : error, invaild arguments passed...\n"
                "try %s -h for help \n",
                exe_name, exe_name);
        exit(1);
    }
    strncat(command, temp, 99);
    system(command);
}
class Engine
{
  private:
    int argc;
    char **argv;

  public:
    void err(const std::string _err, int _exit = 1)
    {
        fmt::print("ERR :{}\n", _err);
        exit(_exit);
    }

    void parse(char **Argv, int Argc){};
};
/* read from a file the dev type and where to mount
 * and can also read aliases
 */

int main(int argc, char **argv)
{

    Engine *E = new Engine;

    if (argc > 2)
    {
        E->err(fmt::format("invaild args passed\n try {} --help for more information\n", program_invocation_name));
    }
    E->parse(argv, argc);

    if (1)
    {
        if (strcmp("-h", *(argv + 1)) == 0)
        {
            fprintf(stdout,
                    "Usage : %s [ options... ]\n"
                    "<device> <directory>   mount a device on "
                    "directory \n"
                    "-h                     print this message \n"
                    "-lsblk                 show block devices "
                    "before running command\n"
                    "Example:\n\t%s sda1 /mnt\n",
                    *(argv), *(argv));
            exit(0);
        }
        else if (strcmp("-lsblk", *(argv + 1)) == 0)
        {
            FILE *fp;
            fp = popen("lsblk", "r");
            char *buffer = (char *)malloc(sizeof(char));
            if (fp == NULL || buffer == NULL)
            {
                fprintf(stderr, "%s : ERR: lsblk failed or failed to allocate space\n", *(argv));
                pclose(fp);
                exit(1);
            }
            while (fread(buffer, sizeof(*buffer), 1, fp) == 1)
            {
                fwrite(buffer, sizeof(*buffer), 1, stdout);
            }
            buffer = (char *)realloc(buffer, (sizeof(char) * 1024));
            if (buffer == NULL)
            {
                fprintf(stderr, "%s : ERR: unable to allocate memory\n", *(argv));
                exit(1);
            }
            printf("mount: ");
            scanf("%s %s", *(argv + 1), buffer);
            mountsd(*(argv + 1), (char *)buffer, *(argv));
            free(buffer);
            pclose(fp);
            exit(0);
        }
        fprintf(stderr,
                "%s : error, invaild arguments passed...\n"
                "Try %s -h for help \n",
                *(argv), *(argv));
        exit(1);
    }
    if (argc == 3)
    {
        mountsd(*(argv + 1), *(argv + 2), *(argv));
        exit(0);
    }
    fprintf(stderr,
            "%s : error, invaild arguments passed...\n"
            "Try %s -h for help \n",
            *(argv), *(argv));
    exit(1);
}
