# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build

This project uses CMake with vcpkg for dependencies (OpenGL, GLFW, GLEW, GLM).

```bash
# Configure (from repo root)
cmake -B build -S .

# Build
cmake --build build

# Run (from build output directory)
./advanced_computer_graphics_final
```

The build system automatically copies `Shaders/`, `Models/`, and `Textures/` next to the binary on every build, so the executable must be run from its output directory (e.g. `build/Debug/` or `build/Release/`).

There are no tests or lint steps.

## Architecture

The engine lives entirely under `src/Libs/Engine/`. Application-specific code is in `src/Libs/Main/`. Entry point is `src/main.cpp`.

**Singleton managers** (accessed via `::instance()`) own all GPU resources:
- `ShaderManager` — loads and caches GLSL shader programs
- `MeshManager` — loads and caches VAO/VBO meshes (built-in primitives like `"sphere"`, `"plane"`, or OBJ files via `getFromFile`)
- `TextureManager` — caches `Texture2D`, `HDREquirectangularTexture`, `BRDFLUTTexture`; `addPBR(name, dir)` loads the standard albedo/ao/metallic/normal/roughness set from a directory
- `MaterialManager` — caches `Material` instances; `addPBR(name, texName)` wires a `PBRMaterial` from textures already in `TextureManager`
- `InputManager` — wraps GLFW keyboard/mouse input

All managers must be populated in `loadAssets()` (in `main.cpp`) before any scene is constructed, because scenes look up assets by string key.

**Scene graph** (`Scene` → `SceneObject`):
- `Scene` is an abstract base holding a flat `std::vector<SceneObject>`, a `Camera`, point/directional lights, and an `HDRCubemap`.
- `SceneObject` is a plain struct: `Transform`, `shared_ptr<Mesh>`, `shared_ptr<Material>`.
- `Transform` computes a model matrix lazily (dirty flag) from position/rotation(Euler°)/scale.
- To add scene content, subclass `Scene` and populate `sceneObjects` in the constructor.

**Rendering** (`Renderer::draw`):
1. **Shadow pass** — renders all objects with the `"shadow"` shader into the directional light's depth FBO.
2. **Lighting pass** — sorts objects by material pointer to minimize shader switches; calls `bindPerFrame` once per material group, then `bindPerObject` per object.
3. **Cubemap pass** — draws the environment map with `GL_LEQUAL` depth.

**Material system**:
- `Material` is an abstract base with two virtual methods: `bindPerFrame(PerFrameContext)` (camera, lights, IBL textures, shadow map) and `bindPerObject(PerObjectContext)` (model matrix).
- `BlinnPhongMaterial` and `PBRMaterial` are the two concrete implementations.
- Shaders are in `Shaders/` and follow pairs: `<name>.vert` / `<name>.frag`. The PBR shader is `pbr.vert`/`pbr.frag`; Blinn-Phong is `blinn-phong.vert`/`blinn-phong.frag`.

**IBL pipeline** (`HDRCubemap`): converts an equirectangular HDR (loaded by `HDREquirectangularTexture`) into a cubemap, then bakes an irradiance map, a prefilter map, and uses a pre-computed BRDF LUT (`BRDFLUTTexture`) — all constructed in `HDRCubemap`'s constructor via offscreen render passes.
