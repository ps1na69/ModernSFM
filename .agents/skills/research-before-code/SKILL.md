---
name: research-before-code
description: Mandatory pre-implementation research checklist. Use before implementing new features, modules, or third-party integrations.
---

# Skill: Research Before Code

## Procedure

1. **Verify Existing Functionality:**
   - Search the project codebase (`D:\WorkFlow\Programming\Projects\ModernSFM`) for related types, functions, or utilities.
   - Prevent duplicating existing helpers or structures.

2. **Verify Dependencies & Exact APIs:**
   - Verify APIs against official documentation or headers.
   - For SDL3 / SDL_GPU: Check official SDL3 headers or Context7 documentation.
   - For Source 1 formats: Cross-reference `docs/SPECIFICATION.md` and clean-room specifications.
   - Explicitly distinguish between verified APIs and assumptions. Never hallucinate signatures.

3. **Verify License Compatibility:**
   - Ensure any new library conforms to permissive terms (MIT, zlib, Apache 2.0, BSD-3, LGPL-2.1 dynamic).
   - Reject non-commercial or restrictive licenses that conflict with ModernSFM's MIT license.

4. **Formulate Smallest Viable Change:**
   - Define minimal interface boundary.
   - Confirm impact on existing CMake targets.
