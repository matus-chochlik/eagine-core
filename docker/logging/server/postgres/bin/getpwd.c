/* Copyright Matus Chochlik.
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt
 */
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, const char** argv) {
    if(argc != 2) {
        return 1;
    }
    const size_t bufsize = 512;
    if(strlen(argv[1]) > bufsize / 2) {
        return 2;
    }
    char buffer[bufsize];
    if(getlogin_r(buffer, bufsize - 1) != 0) {
        if((getuid() != 0) && (getuid() != 999)) {
            return 3;
        }
    } else {
        if(strncmp(buffer, "postgres", 9) != 0) {
            return 4;
        }
    }
    buffer[0] = '\0';
    strncat(buffer, "/run/secrets/", bufsize - strlen(buffer) - 1);
    strncat(buffer, argv[1], bufsize - strlen(buffer) - 1);
    strncat(buffer, ".passwd", bufsize - strlen(buffer) - 1);
    setuid(0);
    int fd = open(buffer, 0);
    if(fd < 0) {
        write(2, buffer, strlen(buffer));
        return 5;
    }
    ssize_t len = read(fd, buffer, bufsize);
    if(len < 0) {
        return 6;
    }
    write(1, buffer, len);
    close(fd);
    return 0;
}
