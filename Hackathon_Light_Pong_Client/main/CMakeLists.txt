idf_component_register(
    SRCS
        "espnow_example_main.c"    

    INCLUDE_DIRS
        "."    

    REQUIRES
        espnow-client
        icm-42688-p
        spi
        led_matrix
        button

)
