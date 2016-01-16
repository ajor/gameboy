add_executable(gb.pexe graphics_2d.cc)
target_link_libraries(gb.pexe  gb_core)
pnacl_finalise(gb.pexe)

target_compile_options(gb.pexe PRIVATE "-std=gnu++14")
target_compile_options(gb_core PRIVATE "-std=gnu++14")
target_link_libraries(gb.pexe ppapi_cpp)
target_link_libraries(gb.pexe ppapi)
target_link_libraries(gb.pexe ppapi_gles2)
