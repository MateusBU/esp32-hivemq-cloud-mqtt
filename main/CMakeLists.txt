idf_component_register(
    SRCS
        "main.c"
        "wifi_manager.c"
        "mqtt_manager.c"
        "config.c"
        "nvs_manager.c"

    INCLUDE_DIRS
        "."
    
    EMBED_TXTFILES 
        "certs/isrg-root-x1-cross-signed.pem"
)