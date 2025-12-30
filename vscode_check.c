#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define NULLDEV " > NUL 2>&1"
#else
#define NULLDEV " > /dev/null 2>&1"
#endif

static int check_command(const char *cmd, const char *name) {
    char buf[512];
    snprintf(buf, sizeof(buf), "%s%s", cmd, NULLDEV);
    int rc = system(buf);
    printf("  - %s: %s\n", name, (rc == 0) ? "FOUND" : "NOT FOUND");
    return rc == 0;
}

int main(void) {
    printf("VSCode C toolchain checker\n");
    printf("Running basic environment checks...\n");

    int ok = 1;

    printf("Checking commands:\n");
    int has_gcc = check_command("gcc --version", "gcc");
    int has_gdb = check_command("gdb --version", "gdb (debugger)");
    int has_git = check_command("git --version", "git");
    int has_code = check_command("code --version", "code (VS Code CLI)");

    if (!has_gcc) {
        printf("\nERROR: `gcc` not found on PATH — install MinGW or GCC toolchain and add to PATH.\n");
        ok = 0;
    }

    /* Try to compile a tiny test program to ensure compile+link works */
    printf("\nTesting compilation: writing and compiling a small C file...\n");
    const char *src_name = "vscode_check_temp.c";
    FILE *f = fopen(src_name, "w");
    if (!f) {
        perror("fopen");
        return 2;
    }
    fprintf(f, "#include <stdio.h>\nint main(void){ printf(\"ok\\n\"); return 0; }\n");
    fclose(f);

    char cmd[512];
#ifdef _WIN32
    snprintf(cmd, sizeof(cmd), "gcc %s -o vscode_check_temp.exe%s", src_name, NULLDEV);
#else
    snprintf(cmd, sizeof(cmd), "gcc %s -o vscode_check_temp%s", src_name, NULLDEV);
#endif
    int compile_ok = (system(cmd) == 0);
    printf("  - compile test: %s\n", compile_ok ? "SUCCESS" : "FAILURE");

    if (compile_ok) {
#ifdef _WIN32
        int run_ok = (system("vscode_check_temp.exe > NUL 2>&1") == 0);
#else
        int run_ok = (system("./vscode_check_temp > /dev/null 2>&1") == 0);
#endif
        printf("  - run test: %s\n", run_ok ? "SUCCESS" : "FAILURE");
        if (!run_ok) ok = 0;
    } else {
        ok = 0;
    }

    /* cleanup */
    remove(src_name);
#ifdef _WIN32
    remove("vscode_check_temp.exe");
#else
    remove("vscode_check_temp");
#endif

    printf("\nSummary:\n");
    printf("  - Essential: gcc compile/run => %s\n", (has_gcc && compile_ok) ? "OK" : "MISSING/FAILED");
    printf("  - Recommended tools: git(%s), gdb(%s), code CLI(%s)\n",
           has_git ? "yes" : "no",
           has_gdb ? "yes" : "no",
           has_code ? "yes" : "no");

    if (ok) {
        printf("\nAll essential checks passed. Your VS Code C toolchain looks set up.\n");
        return 0;
    } else {
        printf("\nOne or more checks failed. See messages above to fix your setup.\n");
        return 1;
    }
}
