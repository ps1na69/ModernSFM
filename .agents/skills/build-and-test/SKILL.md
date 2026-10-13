---
name: build-and-test
description: Verifies compilation, executes test targets, and validates static analysis for ModernSFM.
---

# Skill: Build and Test

## Procedure

1. **Verify Environment Configuration:**
   - Visual Studio 2026 MSVC (`14.51.36231`) x64.
   - LLVM tools located at `C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\Llvm\x64\bin` (`clang-format.exe`, `clang-tidy.exe`).

2. **Configure CMake Target:**
   ```powershell
   cmake -B build -S . -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
   ```

3. **Compile Codebase:**
   ```powershell
   cmake --build build --config Release
   ```

4. **Run Automated Tests:**
   ```powershell
   ctest --test-dir build --output-on-failure -C Release
   ```

5. **Static Analysis & Formatting:**
   - Format check: `& "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\Llvm\x64\bin\clang-format.exe" --dry-run --Werror <files>`
   - Tidy check: `& "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\Llvm\x64\bin\clang-tidy.exe" -p build <files>`

6. **Report Exact Results:**
   - Never report success without verifying the process exit code (`0`).
   - Include any warnings or compilation notes.
