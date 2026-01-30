idf_component_register(
    SRCS
        "client.c"    

    INCLUDE_DIRS
        "."    

    REQUIRES
        espnow-client
        icm-42688-p
        spi
        led_matrix
        button

)
