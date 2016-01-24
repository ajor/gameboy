target_compile_options(gb_core PRIVATE "-std=gnu++14")

# 2D graphics
add_executable(gb2.pexe graphics_2d.cc)
target_link_libraries(gb2.pexe gb_core)
pnacl_finalise(gb2.pexe)
pnacl_compress(gb2.pexe)

target_compile_options(gb2.pexe PRIVATE "-std=gnu++14")
target_link_libraries(gb2.pexe ppapi_cpp)
target_link_libraries(gb2.pexe ppapi)
target_link_libraries(gb2.pexe ppapi_gles2)

# 3D graphics
add_executable(gb3.pexe graphics_3d.cc)
target_link_libraries(gb3.pexe gb_core)
pnacl_finalise(gb3.pexe)
pnacl_compress(gb3.pexe)

target_compile_options(gb3.pexe PRIVATE "-std=gnu++14")
target_link_libraries(gb3.pexe ppapi_cpp)
target_link_libraries(gb3.pexe ppapi)
target_link_libraries(gb3.pexe ppapi_gles2)
