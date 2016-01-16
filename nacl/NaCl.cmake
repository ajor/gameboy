SET(NACL                 ON)
SET(CMAKE_SYSTEM_NAME    NaCl)
SET(CMAKE_C_COMPILER     ${NACL_TOOLCHAIN_BIN}/pnacl-clang)
SET(CMAKE_CXX_COMPILER   ${NACL_TOOLCHAIN_BIN}/pnacl-clang++)
SET(CMAKE_AR             ${NACL_TOOLCHAIN_BIN}/pnacl-ar CACHE STRING "")
SET(CMAKE_RANLIB         ${NACL_TOOLCHAIN_BIN}/pnacl-ranlib STRING "")

SET(CMAKE_FIND_ROOT_PATH ${NACL_TOOLCHAIN_BIN})

SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

include_directories(${NACL_TOOLCHAIN_BIN}/../../../include)
link_directories(${NACL_TOOLCHAIN_BIN}/../../../lib/pnacl/Release)

macro(pnacl_finalise _target)
  add_custom_command(TARGET ${_target} POST_BUILD
    COMMENT "Finalising ${_target}"
    COMMAND "${NACL_TOOLCHAIN_BIN}/pnacl-finalize" "$<TARGET_FILE:${_target}>")
endmacro()
