# ModernSFM — Initial Research Report & Engineering Specification

**Document Version:** 1.0.0  
**Date:** 2026-10-13  
**Status:** Approved for Implementation (Milestone M0 Deliverable)  
**Target Repository:** `D:\WorkFlow\Programming\Projects\ModernSFM`  

---

## 1. Verified Environment Report

### 1.1 Host Environment
- **Operating System:** Microsoft Windows 11 Enterprise / Pro (64-bit, build 10.0.26300-SP0)
- **Host Architecture:** AMD64 (x64 native)
- **Python Runtime:** Python 3.15.0 x64 (MSC v.1951 64-bit, AMD64, installed at system level)
- **Build System:** CMake 4.4.4
- **Version Control:** Git 2.56.0.windows.2
- **C/C++ Compiler:** Visual Studio Community 2026 (MSVC v14.51.36231 / Dev18 toolset)
- **Target Architecture:** Windows x64 (`x86_64-pc-windows-msvc`)
- **Language Standard:** C++23 (`/std:c++latest` or `/std:c++23`)

---

## 2. Repository & Dependency Inventory

To guarantee reproducible, hermetic builds on any clean developer machine without external package manager prerequisites, third-party libraries will be managed via **CMake FetchContent** with pinned Git tags/commits.

| Component | Target Version | Delivery Method | Purpose |
| :--- | :--- | :--- | :--- |
| **SDL3** | `release-3.2.0` (or `3.4.x` branch) | CMake FetchContent | Windowing, Win32 events, input abstraction, high-DPI |
| **SDL_GPU** | Included with SDL3 | Core SDL3 API | Modern hardware abstraction layer (Direct3D 12, Vulkan) |
| **Dear ImGui** | `docking` branch | CMake FetchContent | DCC user interface, dockable viewport, editors |
| **nanobind** | `v2.4.0+` (C++20/23) | CMake FetchContent | Lightweight, high-performance Python 3.15 bindings |
| **glm** | `1.0.1` | CMake FetchContent | Matrix, quaternion, and vector mathematics |
| **spdlog / fmt** | `v1.14.1` | CMake FetchContent | Thread-safe structured diagnostic logging |
| **doctest** | `v2.4.11` | CMake FetchContent | Fast C++ automated unit & regression testing |

---

## 3. License & Redistribution Matrix

Compliance with all third-party intellectual property licenses is mandatory. The project itself is licensed under the permissive **MIT License**.

| Dependency / Reference | Upstream Author | Upstream License | Usage in ModernSFM | Redistribution & Attribution Obligations |
| :--- | :--- | :--- | :--- | :--- |
| **ModernSFM** | ps1na69 | **MIT** | Project Core | Include copyright notice in distributions |
| **SDL3 & SDL_GPU** | libsdl-org / Valve | **zlib License** | Compiled static/dynamic library | Keep zlib notice in documentation / binaries |
| **Dear ImGui** | Omar Cornut | **MIT License** | Compiled static library | Include MIT copyright header |
| **nanobind** | Wenzel Jakob | **BSD 3-Clause** | Header & compiled module | Include BSD 3-Clause notice |
| **glm** | G-Truc Creation | **Happy Bunny / MIT** | Header-only math | Include license notice |
| **spdlog** | Gabi Melman | **MIT License** | Compiled logging library | Include MIT copyright notice |
| **SourceIO** | REDxEYE | **MIT License** | Format specification reference | Safe clean-room specification reference |
| **Source SDK 2013** | Valve Corporation | **Source SDK License** | **Reference ONLY** | **NO PROPRIETARY CODE COPIED.** Clean-room implementation only. |
| **Crowbar** | ZeqMacaw | **CC BY-NC-SA 4.0** | **Reference ONLY** | Non-commercial restriction; no code copying. |
| **VTFLib** | Neil Jedrzejewski | **LGPL 2.1** | Format layout reference | Clean-room parser implemented in C++ |

All third-party notices will be compiled into `THIRD_PARTY_NOTICES.md` at the repository root.

---

## 4. Source-Format Compatibility Matrix

ModernSFM focuses on reproducing compatibility with classic Source 1 (Orange Box / Source 2013 / SFM branch) data structures:

| Format | Version | Magic / Signature | ModernSFM Support Level | Key Technical Caveats & Requirements |
| :--- | :--- | :--- | :--- | :--- |
| **MDL** | 48 | `IDST` (`0x54534449`) | **Full (Native SFM)** | Checksum must match `.vvd` and `.vtx`. Reads bones, attachments, bodyparts. |
| **MDL** | 44, 49 | `IDST` | **Read & Auto-Convert** | Version 44 lacks secondary headers; version 49 has `studiohdr2_t`. Auto-adapt. |
| **VVD** | 4 | `IDSV` (`0x56534449`) | **Full** | **Critical:** Must evaluate `vertexFileFixup_t` table when `numFixups > 0`. |
| **VTX** | 7 (`.dx90.vtx`) | Header Ver 7 | **Full** | Resolve index buffer indirection via `StripGroupHeader_t.origMeshVertID`. |
| **VTF** | 7.0–7.4 | `VTF\0` (`0x00465456`) | **Full Native** | DXT1, DXT5, BGR888, BGRA8888, UV88. Decode mipmaps stored smallest-to-largest. |
| **VTF** | 7.5 | `VTF\0` | **Supported (Auto-patch)** | Auto-downgrade byte `0x08` for compatibility if no extended resources are used. |
| **VMT** | KV1 | ASCII / UTF-8 | **Full KeyValues 1 Parser** | Resolves `$basetexture`, `$bumpmap`, `$phong`, `$rimlight`, color vectors/tints. |
| **DMX** | Binary 2–5 | Header comment | **Full** | `format sfm_session 1 encoding binary 5`. Reads tracks, shots, channels, curves. |
| **DMX** | KV2 (Text) | `keyvalues2 1` | **Full Read/Write** | Text exchange for debugging, diffing, and source assets. |

---

## 5. Architectural Proposal & Target Decomposition

ModernSFM is designed around a strictly decoupled, modular architecture with explicit interface boundaries:

```
┌─────────────────────────────────────────────────────────────┐
│                      modern_sfm_editor                      │
│        (ImGui Docking UI, Animation Set, Timeline, Graph)   │
└──────────────┬──────────────────────────────┬───────────────┘
               │                              │
┌──────────────▼──────────────┐ ┌─────────────▼───────────────┐
│     scene_runtime / anim    │ │          scripting          │
│ (ECS / Graph / IK / Curves) │ │   (Python 3.15 + nanobind)  │
└──────────────┬──────────────┘ └─────────────┬───────────────┘
               │                              │
┌──────────────▼──────────────┐               │
│        source_assets        │               │
│ (MDL, VVD, VTX, VTF, DMX)   │               │
└──────────────┬──────────────┘               │
               │                              │
┌──────────────▼──────────────┐               │
│           renderer          │               │
│     (SDL_GPU Abstraction)   │               │
└──────────────┬──────────────┘               │
               │                              │
┌──────────────▼──────────────────────────────▼───────────────┐
│                    platform / core / math                   │
│         (SDL3 Base, Win32, Thread Pool, Memory, GLM)        │
└─────────────────────────────────────────────────────────────┘
```

### Module Structure
1. **`core`**: Memory allocators, generational handles, thread pool, logging (`spdlog`), assertions.
2. **`platform`**: SDL3 lifecycle wrapper, window creation, native event pumps, high-DPI scaling.
3. **`renderer`**: SDL_GPU graphics pipeline, swapchain management, dynamic vertex/index buffers, shader binding, Dear ImGui SDL_GPU backend integration.
4. **`source_assets`**: Clean-room parsers for MDL, VVD (with fixup unbundler), VTX, VTF (with BC1-BC3 decompressors), VMT KeyValues 1 parser, and DMX serializer.
5. **`scene_runtime`**: Scene graph, camera controllers, lights, mesh instance transforms, bone hierarchies.
6. **`animation`**: Evaluator for skeletal tracks, RPN flex evaluation (`mstudioflexop_t`), Bezier curve interpolation.
7. **`timeline`**: Multi-track sequencer (Film clips, shot boundaries, audio synchronization).
8. **`scripting`**: Python 3.15 runtime lifecycle, isolated PEP 587 `PyConfig`, transactional undo-guarded nanobind module (`sfm`).
9. **`project`**: DMX session serialization, deterministic project save/load, auto-recovery.
10. **`editor`**: Desktop application shell with Dear ImGui docking (Viewport, Animation Set Editor, Clip Editor, Graph Editor, Element Viewer).

---

## 6. Milestone Plan & Acceptance Criteria

### Milestone M0: Foundations & Environment Verification (Current)
- **Objective:** Repository initialization, dependency configuration, specification documents.
- **Acceptance Criteria:** Verified compiler/CMake tools, clean documentation tree, CMake skeleton compiling with SDL3 + SDL_GPU + ImGui.

### Milestone M1: Minimal Prototype Viewport (Vertical Slice 1)
- **Objective:** Create native window, initialize SDL_GPU device, render Dear ImGui docking shell and a 3D grid viewport with an orbital camera.
- **Acceptance Criteria:** Application starts at 60+ FPS, window resizes cleanly without crashes, ImGui docking functional, swapchain texture handles minimization without device loss.

### Milestone M2: Source 1 Static Mesh Importer (Vertical Slice 2)
- **Objective:** Implement `source_assets` parsing for MDL + VVD + VTX. Upload geometry to SDL_GPU vertex/index buffers.
- **Acceptance Criteria:** Parse a valid TF2/SFM model (e.g. `scout.mdl` or a test prop); correctly unpack VVD fixup table; render the untextured mesh in the viewport.

### Milestone M3: Materials & Textures (Vertical Slice 3)
- **Objective:** Implement VTF decoder (DXT1/DXT5/RGBA) and VMT KeyValues parser.
- **Acceptance Criteria:** Load diffuse `$basetexture` and normal maps; display shaded model with basic Phong/Half-Lambert lighting in SDL_GPU.

### Milestone M4: Skeletal Rigging & Animation Set Editor
- **Objective:** Parse bone hierarchies and bind poses from MDL; render skeletal bones; build ImGui Animation Set Editor.
- **Acceptance Criteria:** User can select bones in the tree, manipulate transforms via 3D gizmos, and deform meshes via CPU/GPU skinning.

### Milestone M5: Facial Flexes & DMX Session Parsing
- **Objective:** Implement flex rule evaluation and binary DMX session loader.
- **Acceptance Criteria:** Load an existing `.dmx` SFM session; animate face sliders in the UI; observe real-time mesh deformation.

### Milestone M6: Timeline, Graph Editor & Python 3.15 Scripting
- **Objective:** Timeline clip editor, curve interpolation, embedded Python REPL with transactional undo context.
- **Acceptance Criteria:** Run Python scripts to create animation sets, scrub the timeline, and inspect elements.

---

## 7. Minimal Prototype Specification (Milestone M1 Plan)

For the next milestone, we will build a verified, executable vertical slice:
1. Root `CMakeLists.txt` configuring C++23, MSVC flags, and CMake FetchContent for SDL3 and Dear ImGui.
2. `core` target providing platform logging.
3. `renderer` target wrapping `SDL_CreateGPUDevice`, swapchain acquisition, and `ImGui_ImplSDLGPU3`.
4. `editor` executable presenting:
   - Resizable window with high-DPI support.
   - ImGui Docking workspace with a Viewport window, Menu bar, and Log console.
   - Basic 3D wireframe grid drawn via SDL_GPU render pass.
