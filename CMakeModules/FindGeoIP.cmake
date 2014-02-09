FIND_PATH ( GEOIP_INCLUDE_DIR NAMES GeoIP.h PATHS /usr/include/ /usr/local/include/ )
FIND_LIBRARY ( GEOIP_LIBRARY NAMES GeoIP PATHS /usr/lib /usr/local/lib )
FIND_FILE ( GEOLITE_COUNTRY_DB NAMES GeoIP/GeoIP.dat PATHS /usr/share/ /usr/local/share/ )
FIND_FILE ( GEOLITE_CITY_DB NAMES GeoIP/GeoLiteCity.dat PATHS /usr/share/ /usr/local/share/ )


IF ( GEOIP_INCLUDE_DIR AND GEOIP_LIBRARY )
    SET ( GEOIP_FOUND TRUE )
ENDIF (  )

IF ( GEOLITE_COUNTRY_DB)
    SET ( GEOLITE_COUNTRY_FOUND TRUE )
ENDIF (  )

IF ( GEOLITE_CITY_DB)
    SET ( GEOLITE_CITY_FOUND TRUE )
ENDIF (  )


IF ( GEOIP_FOUND )
    MESSAGE ( STATUS "Found GeoIP headers in ${GEOIP_INCLUDE_DIR}" )
    MESSAGE ( STATUS "Found GeoIP library: ${GEOIP_LIBRARY}" )
ELSE (  )
    IF ( GEOIP_FIND_REQUIRED )
        MESSAGE ( FATAL_ERROR "Could not find GeoIP" )
    ENDIF (  )
ENDIF (  )


IF ( GEOLITE_COUNTRY_FOUND )
    MESSAGE ( STATUS "Found GeoLite-Country database: ${GEOLITE_COUNTRY_DB}" )
ELSE (  )
    IF ( GEOLITE_COUNTRY_FIND_REQUIRED )
        MESSAGE ( FATAL_ERROR "Could not find GeoLite-Country database" )
    ENDIF (  )
ENDIF (  )


IF ( GEOLITE_CITY_FOUND )
    MESSAGE ( STATUS "Found GeoLite-City database: ${GEOLITE_CITY_DB}" )
ELSE (  )
    IF ( GEOLITE_CITY_FIND_REQUIRED )
        MESSAGE ( FATAL_ERROR "Could not find GeoLite-City database" )
    ENDIF (  )
ENDIF (  )


