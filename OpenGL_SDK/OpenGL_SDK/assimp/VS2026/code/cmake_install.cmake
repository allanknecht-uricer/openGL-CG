# Install script for directory: C:/OpenGL_SDK/assimp/code

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/Assimp")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "libassimp6.0.4-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/OpenGL_SDK/assimp/VS2026/lib/Debug/assimp-vc145-mtd.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/OpenGL_SDK/assimp/VS2026/lib/Release/assimp-vc145-mt.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/OpenGL_SDK/assimp/VS2026/lib/MinSizeRel/assimp-vc145-mt.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/OpenGL_SDK/assimp/VS2026/lib/RelWithDebInfo/assimp-vc145-mt.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "libassimp6.0.4" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/OpenGL_SDK/assimp/VS2026/bin/Debug/assimp-vc145-mtd.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/OpenGL_SDK/assimp/VS2026/bin/Release/assimp-vc145-mt.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/OpenGL_SDK/assimp/VS2026/bin/MinSizeRel/assimp-vc145-mt.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/OpenGL_SDK/assimp/VS2026/bin/RelWithDebInfo/assimp-vc145-mt.dll")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp" TYPE FILE FILES
    "C:/OpenGL_SDK/assimp/code/../include/assimp/anim.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/aabb.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/ai_assert.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/camera.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/color4.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/color4.inl"
    "C:/OpenGL_SDK/assimp/VS2026/code/../include/assimp/config.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/ColladaMetaData.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/commonMetaData.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/defs.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/cfileio.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/light.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/material.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/material.inl"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/matrix3x3.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/matrix3x3.inl"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/matrix4x4.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/matrix4x4.inl"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/mesh.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/ObjMaterial.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/pbrmaterial.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/GltfMaterial.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/postprocess.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/quaternion.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/quaternion.inl"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/scene.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/metadata.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/texture.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/types.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/vector2.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/vector2.inl"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/vector3.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/vector3.inl"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/version.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/cimport.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/AssertHandler.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/importerdesc.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/Importer.hpp"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/DefaultLogger.hpp"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/ProgressHandler.hpp"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/IOStream.hpp"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/IOSystem.hpp"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/Logger.hpp"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/LogStream.hpp"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/NullLogger.hpp"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/cexport.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/Exporter.hpp"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/DefaultIOStream.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/DefaultIOSystem.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/ZipArchiveIOSystem.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/SceneCombiner.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/fast_atof.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/qnan.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/BaseImporter.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/Hash.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/MemoryIOWrapper.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/ParsingUtils.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/StreamReader.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/StreamWriter.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/StringComparison.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/StringUtils.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/SGSpatialSort.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/GenericProperty.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/SpatialSort.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/SkeletonMeshBuilder.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/SmallVector.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/SmoothingGroups.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/SmoothingGroups.inl"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/StandardShapes.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/RemoveComments.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/Subdivision.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/Vertex.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/LineSplitter.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/TinyFormatter.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/Profiler.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/LogAux.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/Bitmap.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/XMLTools.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/IOStreamBuffer.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/CreateAnimMesh.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/XmlParser.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/BlobIOSystem.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/MathFunctions.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/Exceptional.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/ByteSwapper.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/Base64.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp/Compiler" TYPE FILE FILES
    "C:/OpenGL_SDK/assimp/code/../include/assimp/Compiler/pushpack1.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/Compiler/poppack1.h"
    "C:/OpenGL_SDK/assimp/code/../include/assimp/Compiler/pstdint.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "C:/OpenGL_SDK/assimp/VS2026/bin/Debug/assimp-vc145-mtd.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "C:/OpenGL_SDK/assimp/VS2026/bin/Release/assimp-vc145-mt.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "C:/OpenGL_SDK/assimp/VS2026/bin/MinSizeRel/assimp-vc145-mt.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "C:/OpenGL_SDK/assimp/VS2026/bin/RelWithDebInfo/assimp-vc145-mt.pdb")
  endif()
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/OpenGL_SDK/assimp/VS2026/code/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
