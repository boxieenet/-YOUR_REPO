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

Interview Notes
---------------
This repository contains a compact, single-file embedded systems demo designed for quick review in interviews. Use these talking points when presenting the project:

- Purpose: simulate a simple sensor -> controller -> actuator pipeline with realistic concerns (noise, filtering, mapping, and message integrity).
- Signal conditioning: `movavg_t` implements a moving-average filter — discuss latency vs smoothing trade-offs and how you'd improve it (e.g., exponential filter, FIR window tuning).
- Actuation mapping: `adc_to_duty()` shows domain conversion; mention calibration and deadband handling in real hardware.
- Robustness: `crc8()` demonstrates a small integrity check; discuss why CRCs are preferred over simple checksums and where you'd apply them in telemetry.
- Diagnostics: the `ring_log_t` is a lightweight on-device circular buffer — useful for post-mortem debug when storage is limited.
- Extendability: explain how you'd replace simulation with real ADC reads, hook PWM registers, or add RTOS tasks.

How to present during an interview
-------------------------------
- Briefly explain the architecture (sensor -> filter -> mapping -> actuator).
- Walk through a few key functions (`simulate_adc`, `movavg_push`, `adc_to_duty`, `crc8`).
- Run the binary to show output and point to the ring buffer sample lines in the `Recent log` section.
- Discuss improvements you would implement for production (unit tests, static analysis, CI build, hardware-in-the-loop tests).

Links
-----
- Build locally: See above for `gcc` commands.

