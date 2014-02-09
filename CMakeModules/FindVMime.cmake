FIND_PATH ( VMIME_INCLUDE_DIR NAMES vmime PATHS /usr/include/ /usr/local/include/ )
FIND_LIBRARY ( VMIME_LIBRARY NAMES vmime PATHS /usr/lib /usr/local/lib ) 


IF ( VMIME_INCLUDE_DIR AND VMIME_LIBRARY )
    SET ( VMIME_FOUND TRUE )
ENDIF (  )


IF ( VMIME_FOUND )
    MESSAGE ( STATUS "Found VMime headers in ${VMIME_INCLUDE_DIR}" )
    MESSAGE ( STATUS "Found VMime library: ${VMIME_LIBRARY}" )
ELSE (  )
    IF ( VMIME_FIND_REQUIRED )
        MESSAGE ( FATAL_ERROR "Could not find VMime" )
    ENDIF (  )
ENDIF (  )


