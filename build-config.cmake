# CORELIB_LINK_TYPE
#   * STATIC
#   * SHARED
SET ( CORELIB_LINK_TYPE STATIC )

SET ( BUILD_STOCKMARKET_REST "YES" )

SET ( CORELIB_BIN_NAME "core" )
SET ( STOCKMARKET_REST_BIN_NAME "stock-market.rest" )


# CMAKE_BUILD_TYPE
#   * None
#   * Debug
#   * Release
#   * RelWithDebInfo
#   * MinSizeRel
#   * GCCMinSizeRel
#   * GCCMaxSpeedRel
#   * GCCNativeMinSizeRel
#   * GCCNativeMaxSpeedRel
#   * ClangMinSizeRel
#   * ClangMaxSpeedRel
#   * ClangNativeMinSizeRel
#   * ClangNativeMaxSpeedRel
IF( NOT CMAKE_BUILD_TYPE )
    SET ( CMAKE_BUILD_TYPE "None" CACHE STRING "" FORCE )
ENDIF(  )

# Reduce executables size by using GNU strip (GNU Binutils)
# Note this is GNU toolchain only
SET ( GCC_STRIP_EXECUTABLES "YES" )


