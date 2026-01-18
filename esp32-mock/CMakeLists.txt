include(FindGit)
find_package(Git)
include(FetchContent)

FetchContent_Declare(
  safe-cstring
  GIT_REPOSITORY https://github.com/essenius/safe-cstring
  GIT_TAG        main
)
FetchContent_MakeAvailable(safe-cstring)

set(COMMON_HEADERS Adafruit_SSD1306.h Client.h EEPROM.h ESP.h FS.h HTTPClient.h HTTPUpdate.h IPAddress.h LittleFS.h Preferences.h PubSubClient.h StringArduino.h WiFi.h WiFiClient.h WiFiCommon.h WiFiClientSecureCommon.h WiFiClientSecure.h Wire.h)
set(COMMON_SOURCES EEPROM.cpp ESP.cpp FS.cpp HTTPClient.cpp HTTPUpdate.cpp IPAddress.cpp LittleFS.cpp Preferences.cpp PubSubClient.cpp WiFI.cpp WiFiCommon.cpp WiFiClientSecureCommon.cpp Wire.cpp)

# ESP32 has no extra headers or sources at this time
set(ESP32_HEADERS)
set(ESP32_SOURCES)
set(ESP8266_HEADERS ESP8266HTTPClient.h ESP8266httpUpdate.h ESP8266WiFi.h)
set(ESP8266_SOURCES ESP8266httpUpdate.cpp ESP8266WiFi.cpp)

# Note that some classes are reused by  the esp8266, e.g., HTTPClient and HTTPUpdate. Mock behavior is the same for both platforms. WiFi is reused too, but has some extra methods in the esp8266 version.

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

function(add_mock_lib target_name)
    cmake_parse_arguments(MOCK "" "PLATFORM_DEFINE" "HEADERS;SOURCES" ${ARGN})

    add_library(${target_name} "")

    target_sources(${target_name}
        PUBLIC  ${COMMON_HEADERS} ${MOCK_HEADERS}
        PRIVATE ${COMMON_SOURCES} ${MOCK_SOURCES}
    )

    target_include_directories(${target_name}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${includeFolders}
    )

    if (MOCK_PLATFORM_DEFINE)
        target_compile_definitions(${target_name}
            PUBLIC ${MOCK_PLATFORM_DEFINE}
        )
    endif()
endfunction()


add_mock_lib(${libName}-esp32  
  HEADERS ${ESP32_HEADERS} 
  SOURCES ${ESP32_SOURCES}
  PLATFORM_DEFINE ARDUINO_ARCH_ESP32)
add_mock_lib(${libName}-esp8266 
  HEADERS ${ESP8266_HEADERS} 
  SOURCES ${ESP8266_SOURCES}
  PLATFORM_DEFINE ARDUINO_ARCH_ESP8266)

add_subdirectory(freertos)
add_subdirectory(sys)

install(TARGETS ${libName}-esp32 ${libName}-esp8266 DESTINATION lib)
install(FILES ${COMMON_HEADERS} DESTINATION include)
install(FILES ${ESP32_HEADERS} DESTINATION include)      
install(FILES ${ESP8266_HEADERS} DESTINATION include) 