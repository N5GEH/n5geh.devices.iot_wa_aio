set(COMPONENT_SRCDIRS "src")
set(COMPONENT_ADD_INCLUDEDIRS "include")

set(COMPONENT_REQUIRES log freertos driver nvs_flash esp_http_server mqtt LTE)

register_component()
