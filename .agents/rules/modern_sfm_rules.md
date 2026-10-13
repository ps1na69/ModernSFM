---
description: Comprehensive rules for C++23, SDL3/SDL_GPU, and architectural best practices in ModernSFM
always_on: true
---

# ModernSFM Development Rules & Code Guidelines

## 1. Research-First Implementation Workflow
For every non-trivial coding task:
1. Understand the exact requested behavior and constraints.
2. Inspect the relevant project files and existing implementations before modifying.
3. Consult official documentation for third-party APIs (SDL3, SDL_GPU, Dear ImGui, nanobind).
4. Never guess or fabricate function signatures, types, or enum values.
5. Implement the smallest complete solution with zero dead or placeholder code.
6. Verify changes using automated tests and build targets.

## 2. C++ Architecture & Style Guidelines
- **Standard:** Modern C++23.
- **Resource Management:** Strict RAII. Never use raw owning pointers (`new`/`delete`). Use smart pointers (`std::unique_ptr`, `std::shared_ptr`) or custom RAII wrappers for platform handles.
- **Modular Design:** Keep library modules strictly decoupled (`core`, `platform`, `renderer`, `source_assets`, `scene_runtime`, `animation`, `scripting`, `editor`).
- **Error Handling:** Strong exception safety or explicit result types (`std::expected` / error codes). Do not swallow errors silently.
- **Diagnostics:** Use structured logging (`spdlog`) rather than `std::cout`/`printf`.

## 3. Safe Refactoring
- Identify all affected callers and interfaces before modifying shared contracts.
- Minimize diff surface area. Keep changes focused and atomic.
- Maintain backwards compatibility with supported Source 1 asset formats (MDL v44-49, VVD, VTX, VTF, VMT, DMX).

## 4. Token-Efficient Coding
- Prefer targeted reads (`view_file` with precise line slices) over massive context dumps.
- Search specifically for definitions and usages before reading files.
- Summarize changes concisely with verifiable file links.
