#include "Engine/HDRCubemap.hpp"
#include "Engine/Mesh/MeshManager.hpp"

#include "GL/glew.h"
#include "stb_image.h"

#include <array>
#include <iostream>
#include <string>

HDRCubemap::HDRCubemap()
    : cube(MeshManager::instance().get("cube"))
{ }