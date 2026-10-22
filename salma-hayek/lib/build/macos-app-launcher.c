/*
 * Terminator-M4 native macOS application launcher.
 * Added 2026-10-22 to provide a valid ARM64 CFBundleExecutable entry point.
 * Distributed with the upstream project; see the repository COPYING file.
 */
#include <limits.h>
#include <mach-o/dyld.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    char executable[PATH_MAX];
    uint32_t executable_size = sizeof(executable);
    if (_NSGetExecutablePath(executable, &executable_size) != 0) {
        fputs("Terminator launcher: executable path is too long\n", stderr);
        return 126;
    }

    char resolved[PATH_MAX];
    if (realpath(executable, resolved) == NULL) {
        perror("Terminator launcher: realpath");
        return 126;
    }

    char script[PATH_MAX];
    int script_length = snprintf(script, sizeof(script), "%s.sh", resolved);
    if (script_length < 0 || (size_t) script_length >= sizeof(script)) {
        fputs("Terminator launcher: script path is too long\n", stderr);
        return 126;
    }

    char **shell_argv = calloc((size_t) argc + 3, sizeof(char *));
    if (shell_argv == NULL) {
        perror("Terminator launcher: calloc");
        return 126;
    }

    shell_argv[0] = "/bin/bash";
    shell_argv[1] = "--login";
    shell_argv[2] = script;
    for (int i = 1; i < argc; ++i) {
        shell_argv[i + 2] = argv[i];
    }

    execv(shell_argv[0], shell_argv);
    perror("Terminator launcher: execv");
    return 127;
}
