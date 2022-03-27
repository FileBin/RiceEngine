cmake_minimum_required (VERSION 3.8)

#project config
project ("GameEngine")
project ("GameMain")

cmake_path(SET GAME_ENGINE_PATH "${CMAKE_SOURCE_DIR}/src/GameEngine")
cmake_path(SET GAME_MAIN_PATH "${CMAKE_SOURCE_DIR}/src/GameMain")
cmake_path(SET LIB_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/lib/include")
cmake_path(APPEND DXSDK_LIB_PATH $ENV{DXSDK_DIR} "lib")
cmake_path(APPEND DXSDK_INCLUDE_PATH $ENV{DXSDK_DIR} "include")
cmake_path(SET SHADER_PATH "${GAME_MAIN_PATH}/shaders")
cmake_path(SET HLSL_INCLUDE_PATH "${SHADER_PATH}/include")
cmake_path(SET RESOURCES_PATH "${CMAKE_SOURCE_DIR}/res/")

#file search
file(GLOB_RECURSE GAME_ENGINE_SRC "${GAME_ENGINE_PATH}/*.h" "${GAME_ENGINE_PATH}/*.cpp")
file(GLOB_RECURSE GAME_MAIN_SRC "${GAME_MAIN_PATH}/*.h" "${GAME_MAIN_PATH}/*.cpp")
file(GLOB_RECURSE LIBS_x64 "${CMAKE_SOURCE_DIR}/lib/x64/*.lib")
file(GLOB_RECURSE DLLs_x64 "${CMAKE_SOURCE_DIR}/lib/x64/*.dll")

file(GLOB_RECURSE HLSL_VS_SHADER_FILES "${SHADER_PATH}/vs/*.hlsl")
file(GLOB_RECURSE HLSL_PS_SHADER_FILES "${SHADER_PATH}/ps/*.hlsl")
file(GLOB_RECURSE RESOURCES "${RESOURCES_PATH}/*.*")
set(HLSL_ALL_SHADER_FILES ${HLSL_VS_SHADER_FILES} ${HLSL_PS_SHADER_FILES})

#global property
set(CMAKE_CXX_STANDARD 20)

#Source config

#GameEngine

#includes
include_directories(GameEngine ${DXSDK_INCLUDE_PATH})
include_directories(GameEngine ${LIB_INCLUDE_PATH})
include_directories(GameEngine "${GAME_ENGINE_PATH}/include")
include_directories(GameEngine "${GAME_ENGINE_PATH}/src")

#build
add_library (GameEngine ${GAME_ENGINE_SRC})
target_precompile_headers(GameEngine PUBLIC "${GAME_ENGINE_PATH}/src/pch.h")

#GameMain
#includes
include_directories(GameMain ${DXSDK_INCLUDE_PATH})
include_directories(GameMain ${LIB_INCLUDE_PATH})
include_directories(GameMain "${GAME_MAIN_PATH}/src")

#build
add_executable (GameMain ${GAME_MAIN_SRC})
target_precompile_headers(GameMain PUBLIC "${GAME_MAIN_PATH}/src/pch.h")


target_link_directories(GameMain PUBLIC
${PROJECT_BINARY_DIR}
"${DXSDK_LIB_PATH}/x64"
)

target_link_libraries(GameMain
    ${LIBS_x64}
    )



#copy dll
add_custom_command(TARGET GameMain POST_BUILD   # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND}
        -E copy_if_different                    # which executes "cmake - E copy_if_different..."
        ${DLLs_x64}                           # <--this is in-file
        ${CMAKE_BINARY_DIR})                  # <--this is out-file path


# Build HLSL shaders
add_custom_target(Shaders ALL DEPENDS ${HLSL_ALL_SHADER_FILES})
set(SHADERS_DIR ${CMAKE_BINARY_DIR}/shaders)
set(SHADERS_PDB_DIR ${CMAKE_BINARY_DIR}/pdb/shaders)

file(MAKE_DIRECTORY ${SHADERS_DIR})
file(MAKE_DIRECTORY ${SHADERS_PDB_DIR})

set_source_files_properties(${HLSL_VS_SHADER_FILES} PROPERTIES ShaderType "vs")
set_source_files_properties(${HLSL_PS_SHADER_FILES} PROPERTIES ShaderType "ps")
set_source_files_properties(${HLSL_ALL_SHADER_FILES} PROPERTIES ShaderModel "5_0")

foreach(FILE ${HLSL_ALL_SHADER_FILES})
  get_filename_component(FILE_WE ${FILE} NAME_WE)
  get_source_file_property(shadertype ${FILE} ShaderType)
  get_source_file_property(shadermodel ${FILE} ShaderModel)
  IF(CMAKE_BUILD_TYPE MATCHES DEBUG)
    set(PARAMS /Od)
  ENDIF(CMAKE_BUILD_TYPE MATCHES DEBUG)
  add_custom_command(TARGET Shaders
                     COMMAND fxc.exe 
                     /nologo /E main 
                     /I ${HLSL_INCLUDE_PATH} 
                     /T ${shadertype}_${shadermodel} 
                     ${PARAMS}
                     /Zi 
                     /Fo ${SHADERS_DIR}/${FILE_WE}.cso 
                     /Fd ${SHADERS_PDB_DIR}/${FILE_WE}.pdb ${FILE}
                     MAIN_DEPENDENCY ${FILE}
                     COMMENT "HLSL ${FILE}"
                     VERBATIM)
endforeach(FILE)

#copy resources
add_custom_target(Resources ALL DEPENDS ${RESOURCES})
set(OUT_RES_DIR ${CMAKE_BINARY_DIR}/resources)

file(MAKE_DIRECTORY ${OUT_RES_DIR})

add_custom_command(TARGET Resources   # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND}
        -E copy_directory                    # which executes "cmake - E copy_if_different..."
        ${RESOURCES_PATH}                           # <--this is in-file
        ${OUT_RES_DIR})          # <--this is out-file path