#include "locator.h"

#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

DYNAMIC_ARRAY_DEF(SongList, song_list, Song, song_to_str, "[", ",\n ", "]", song_free);

void * xmalloc(size_t size)
{
    register void *value = malloc(size);
    if (value == NULL) {
        fprintf(stderr, "virtual memory exhausted");
        exit(1);
    }
    return value;
}

static bool is_mp3(char *abspath)
{
    size_t len = strlen(abspath);
    return len >= 4 && strcmp(abspath + len - 4, ".mp3") == 0;
}

int get_songs_abs(const char *dirpath, SongList *sl)
{
    DIR *dir;
    if ((dir = opendir(dirpath)) == NULL) {
        char err[200] = "Cannot open directory ";
        add_str(dirpath, err + strlen(err), 200 - strlen(err));
        perror(err);
        return errno;
    }

    struct dirent *d;
    while ((d = readdir(dir)) != NULL) {
        // ignore this directory and parent directory
        if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0) continue;

        size_t fullpathlen = strlen(dirpath) + 1 + strlen(d->d_name) + 1;
        char *fullpath = (char *)xmalloc(fullpathlen);
        int sz = snprintf(fullpath, fullpathlen, "%s/%s", dirpath, d->d_name);
        assert(sz == fullpathlen - 1);

        char *abspath = canonicalize_file_name(fullpath);
        free(fullpath);

        struct stat finfo;
        if (lstat(abspath, &finfo) != 0) {
            char err[200] = "Can't lstat file ";
            add_str(abspath, err + strlen(err), 200 - strlen(err));
            perror(err);
            free(abspath);
            closedir(dir);
            return errno;
        }

        assert(S_ISLNK(finfo.st_mode) == 0);
        if (S_ISDIR(finfo.st_mode)) {
            // recurse
            if (get_songs_abs(abspath, sl) != 0) {
                free(abspath);
                closedir(dir);
                return errno;
            }
        } else if (S_ISREG(finfo.st_mode) && is_mp3(abspath)) {
            Song s;
            s.filepath = char_arr_to_string(abspath);
            s.name = char_arr_to_string(d->d_name);
            // remove .mp3 extension
            assert(s.name.count >= 4);
            s.name.count -= 4;
            song_list_add(sl, &s);
        }
        free(abspath);
    }
    closedir(dir);

    // at this point, either all files were traversed successfully, or some
    // error occurred reading something
    if (errno != 0) {
        perror("Error reading file");
        return errno;
    } else
        return 0;
}

/*
int main (int argc, char *argv[])
{
    printf("File type constants:\n"
           "\tDT_UNKNOWN = %d\n"
           "\tDT_REG = %d\n"
           "\tDT_DIR = %d\n"
           "\tDT_FIFO = %d\n"
           "\tDT_SOCK = %d\n"
           "\tDT_CHR = %d\n"
           "\tDT_BLK = %d\n"
           "\tDT_LNK = %d\n", DT_UNKNOWN, DT_REG, DT_DIR, DT_FIFO, DT_SOCK,
           DT_CHR, DT_BLK, DT_LNK);

    SongList sl;
    song_list_init(&sl);
    get_songs_abs(argv[1], &sl);

    printf("%zu songs found\n", sl.count);

    char result[5001] = {0};
    song_list_to_str(&sl, result, 5000);
    printf("%s\n", result);
    song_list_free(&sl);
    return 0;
}
*/
