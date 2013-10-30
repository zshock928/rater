#include "locator.h"

#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

static gboolean is_mp3(gchar *abspath)
{
    size_t len = strlen(abspath);
    return len >= 4 && strcmp(abspath + len - 4, ".mp3") == 0;
}

gint get_songs_abs(const gchar *dirpath, GSList **songs)
{
    *songs = NULL;

    DIR *dir;
    if ((dir = opendir(dirpath)) == NULL) {
        GString *err = g_string_new("opendir(\"");
        g_string_append(err, dirpath);
        g_string_append(err, "\")");
        perror(err->str);
        g_string_free(err, TRUE);
        return errno;
    }

    struct dirent *d;
    while ((d = readdir(dir)) != NULL) {
        // ignore this directory and parent directory
        if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0) continue;

        GString *fullpath = g_string_new(dirpath);
        g_string_append(fullpath, "/");
        g_string_append(fullpath, d->d_name);
        char *abspath = canonicalize_file_name(fullpath->str);
        g_string_free(fullpath, TRUE);

        struct stat finfo;
        if (lstat(abspath, &finfo) != 0) {
            GString *err = g_string_new("lstat(\"");
            g_string_append(err, abspath);
            g_string_append(err, "\")");
            perror(err->str);
            g_string_free(err, TRUE);
            free(abspath);
            closedir(dir);
            return errno;
        }

        assert(S_ISLNK(finfo.st_mode) == 0);
        if (S_ISDIR(finfo.st_mode)) {
            // recurse
            if (get_songs_abs(abspath, songs) != 0) {
                free(abspath);
                closedir(dir);
                return errno;
            }
        } else if (S_ISREG(finfo.st_mode) && is_mp3(abspath)) {
            Song *s = g_new(Song, 1);
            song_init(s, g_string_new(abspath), g_string_new(d->d_name));

            // remove .mp3 extension
            assert(s->name->len >= 4);
            g_string_truncate(s->name, s->name->len - 4);
            *songs = g_slist_prepend(*songs, s);
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

// Testing
/*
gint main (gint argc, gchar *argv[])
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

    GSList *songs;
    gint err = get_songs_abs(argv[1], &songs);
    if (err) return err;
    if (songs == NULL) {
        printf("No songs found in %s", argv[1]);
        return 0;
    }

    GSList *song_it;
    for (song_it = songs; song_it; song_it = song_it->next) {
        GString *result = song_to_str(song_it->data);
        puts(result->str);
        g_string_free(result, TRUE);
        song_free(song_it->data);
        g_free(song_it->data);
    }
    g_slist_free(songs);
    return 0;
}
*/
