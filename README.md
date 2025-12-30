# VS Code C Toolchain Check

This small program helps verify a VS Code C development setup on Windows (MinGW) or Unix-like systems.

Files created:

- `vscode_check.c` — the checker program

How to build and run (Windows + MinGW):

```powershell
Push-Location 'e:\c projects program'
C:\MinGW\bin\gcc.exe -O2 vscode_check.c -o vscode_check.exe
.\vscode_check.exe
Pop-Location
```

Or a simple command (if `gcc` is on PATH):

```powershell
gcc -O2 "vscode_check.c" -o "vscode_check.exe"
.\vscode_check.exe
```

On Unix/macOS:

```bash
gcc -O2 vscode_check.c -o vscode_check
./vscode_check
```

Interpretation:
- Exit code 0: essential checks passed (gcc found, test program compiled and ran).
- Exit code 1: one or more checks failed; review the printed output.
- Exit code 2: I/O error writing the temporary test file.

Next steps:
- If `gcc` is missing, install a C toolchain (MinGW on Windows, build-essential on Debian/Ubuntu, Xcode command line tools on macOS).
- If `code` (VS Code CLI) is missing but VS Code is installed, enable the `code` command from the Command Palette (`Shell Command: Install 'code' command in PATH`).
