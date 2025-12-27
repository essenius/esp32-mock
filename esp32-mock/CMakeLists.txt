include(FindGit)
find_package(Git)
include(FetchContent)

FetchContent_Declare(
  safe-cstring
  GIT_REPOSITORY https://github.com/essenius/safe-cstring
  GIT_TAG        main
)
FetchContent_MakeAvailable(safe-cstring)

set(COMMON_HEADERS Adafruit_SSD1306.h Client.h EEPROM.h ESP.h FS.h HTTPClient.h HTTPUpdate.h IPAddress.h Preferences.h PubSubClient.h StringArduino.h WiFiClient.h WiFiCommon.h WiFiClientSecureCommon.h WiFiClientSecure.h Wire.h)
set(COMMON_SOURCES EEPROM.cpp ESP.cpp FS.cpp HTTPClient.cpp HTTPUpdate.cpp IPAddress.cpp Preferences.cpp PubSubClient.cpp WiFiCommon.cpp WiFiClientSecureCommon.cpp WiFiClientSecure.cpp Wire.cpp)

set(ESP32_HEADERS WiFi.h)
set(ESP8266_HEADERS ESP8266WiFi.h)
set(ESP32_SOURCES WiFi.cpp)
set(ESP8266_SOURCES ESP8266WiFi.cpp)

# If we build under vcpkg, we add the include folder 
MESSAGE(STATUS "CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}")
if (DEFINED CMAKE_PREFIX_PATH) 
  foreach (entry ${CMAKE_PREFIX_PATH})
      list(APPEND includeFolders ${entry}/include)
  endforeach() 
# Otherwise, we directly add the dependency include folder (it's header only)
else()
  list(APPEND includeFolders ${safe-cstring_SOURCE_DIR}/src)
endif()

function(add_mock_lib target_name platform_define platform_headers platform_sources)
  add_library(${target_name} "")

  target_sources(${target_name}
    PUBLIC  ${COMMON_HEADERS} ${platform_headers}
    PRIVATE ${COMMON_SOURCES} ${platform_sources})

  target_include_directories(${target_name}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${includeFolders})

  target_compile_definitions(${target_name}
    PUBLIC ${platform_define})
endfunction()

add_mock_lib(${libName}-esp32 ARDUINO_ARCH_ESP32 ${ESP32_HEADERS} ${ESP32_SOURCES})
add_mock_lib(${libName}-esp8266 ARDUINO_ARCH_ESP8266 ${ESP8266_HEADERS} ${ESP8266_SOURCES})

add_subdirectory(freertos)
add_subdirectory(sys)

install(TARGETS ${libName}-esp32 ${libName}-esp8266 DESTINATION lib)
install(FILES ${COMMON_HEADERS} DESTINATION include)
install(FILES ESP32_HEADERS DESTINATION include)      
install(FILES ESP8266_HEADERS DESTINATION include) 