FIND_PATH ( CURLPP_INCLUDE_DIR NAMES curlpp PATHS /usr/include/ /usr/local/include/ )
FIND_LIBRARY ( CURLPP_LIBRARY NAMES curlpp PATHS /usr/lib /usr/local/lib ) 


IF ( CURLPP_INCLUDE_DIR AND CURLPP_LIBRARY )
    SET ( CURLPP_FOUND TRUE )
ENDIF (  )


IF ( CURLPP_FOUND )
    MESSAGE ( STATUS "Found cURLpp headers in ${CURLPP_INCLUDE_DIR}" )
    MESSAGE ( STATUS "Found cURLpp library: ${CURLPP_LIBRARY}" )
ELSE (  )
    IF ( CURLPP_FIND_REQUIRED )
        MESSAGE ( FATAL_ERROR "Could not find cURLpp" )
    ENDIF (  )
ENDIF (  )


