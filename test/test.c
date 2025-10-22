#include "test.h"
#include <dirent.h>
#include <dlfcn.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define TEST_DIR "./bin/"

int main(void)
{
        DIR *dir = opendir(TEST_DIR);
        if (!dir) {
                perror("opendir");
                return 1;
        }
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
                // filter out non-library
                const char *name = entry->d_name;
                size_t len = strlen(name);
                if (len < 4 || strcmp(name + len - 3, ".so") != 0)
                        continue;
                // open lib
                char path[512];
                snprintf(path, sizeof(path), "%s%s", TEST_DIR, name);
                void *handle = dlopen(path, RTLD_LAZY);
                if (!handle) {
                        fprintf(stderr, "> Failed to load %s: %s\n", path, dlerror());
                        continue;
                }
                result_t (*test_func)() = (result_t (*)())(intptr_t)dlsym(handle, "test");
                char *err = dlerror();
                if (err) {
                        fprintf(stderr, "> Failed to find symbol in %s: %s\n", path, dlerror());
                        dlclose(handle);
                        continue;
                }
                result_t r = test_func();
                printf("  <%s>: [%d/%d]\n", name, r.passed, r.total);
                dlclose(handle);
        }
        closedir(dir);
        return 0;
}
