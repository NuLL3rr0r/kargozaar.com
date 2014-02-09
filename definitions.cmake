SET ( STOCK_DATA_DB_FILE_NAME "stockdata.db" )
SET ( STOCK_DATA_LOCAL_TEMP_FILE "stock-latest.xlsx" )
SET ( STOCK_DATA_SOURCE_URL "http://www.tsetmc.com/tsev2/excel/MarketWatchPlus.aspx?d=0" )
SET ( STOCK_DATA_TEMP_WORK_DIR "stock-excel-temp" )
SET ( STOCK_DATA_UPDATE_SECONDS_INTERVAL "120" )

SET ( LIBB64_BUFFERSIZE "16777216" )

IF ( WIN32 )
    SET ( CORELIB_DEFINES "_UNICODE; UNICODE; WIN32_LEAN_AND_MEAN" )
ENDIF (  )

IF ( WIN32 )
    SET ( STOCKMARKET_REST_DEFINES "_UNICODE; UNICODE; WIN32_LEAN_AND_MEAN" )
ENDIF (  )


