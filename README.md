# P-Machine Virtual Machine (PM/0)

A lean implementation of the **P-Machine** in ANSI C.  
Built for COP 3402 — Systems Software (Fall 2025).

---

## Core
- Implements the **PM/0 instruction set**: `LIT, OPR, LOD, STO, CAL, INC, JMP, JPC, SYS`.
- **Fetch–execute cycle** runs from text segment, manipulates stack and registers (`PC`, `BP`, `SP`, `IR`).
- Stack grows downward in a fixed PAS (500 cells).
- Traces execution after every instruction.  
- Runs on **Eustis** server using `gcc`.

---

## Files
- `vm.c` — source code of the virtual machine:contentReference[oaicite:4]{index=4}.  
- `input.txt` — sample program input:contentReference[oaicite:5]{index=5}.  
- `sampleOut.txt` — reference execution trace:contentReference[oaicite:6]{index=6}.  

---

## Build & Run
Compile and execute on Eustis (or any ANSI C environment):

```bash
gcc -O2 -Wall -std=c11 -o vm vm.c
./vm input.txt
