# ModernSFM

**ModernSFM** is an open-source, native Windows x64 3D animation editor designed to reproduce the classic Source Filmmaker workflow with modern technologies:

- **Language:** C++23 (MSVC Dev18 / Clang)
- **Windowing & Input:** SDL3 (3.2.0+)
- **Graphics Backend:** SDL_GPU (Direct3D 12 & Vulkan native)
- **UI Framework:** Dear ImGui (Docking branch)
- **Scripting Runtime:** Python 3.15 with `nanobind`
- **Asset Support:** Source 1 MDL (v44–49), VVD, VTX, VTF (7.0–7.5), VMT, and DMX sessions

---

## Documentation

- [Master Engineering Specification & Research Report](docs/SPECIFICATION.md)
- [Third-Party Notices & Licenses](THIRD_PARTY_NOTICES.md)

---

## Development Milestones

- **M0:** Architecture Specification & Environment Setup *(Completed)*
- **M1:** Minimal Prototype Viewport & ImGui Docking *(In Progress)*
- **M2:** Source 1 Static Mesh Importer (MDL, VVD, VTX)
- **M3:** Shading & Materials (VTF, VMT, PBR/Phong)
- **M4:** Skeletal Animation & Rigging Controls
- **M5:** Facial Flexes & DMX Session Serialization
- **M6:** Sequencer Timeline & Python 3.15 Integration
