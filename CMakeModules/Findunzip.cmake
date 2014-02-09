FIND_FILE ( UNZIP_BINARY NAMES unzip PATHS /usr/bin/ /usr/local/bin/ )


IF ( UNZIP_BINARY )
    SET ( UNZIP_FOUND TRUE )
ENDIF (  )


IF ( UNZIP_FOUND )
    MESSAGE ( STATUS "Found unzip binary in ${UNZIP_BINARY}" )
ELSE (  )
    IF ( UNZIP_FIND_REQUIRED )
        MESSAGE ( FATAL_ERROR "Could not find unzip" )
    ENDIF (  )
ENDIF (  )


