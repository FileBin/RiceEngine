mkdir -p bin
glslc triangle.frag -o ./bin/triangle.frag.spv -fshader-stage=frag
glslc triangle.vert -o ./bin/triangle.vert.spv -fshader-stage=vert