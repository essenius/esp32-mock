include(FindGit)
find_package(Git)

message(STATUS "Building Tests for ${libName}")

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG release-1.12.1
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
    INSTALL_COMMAND ""
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

set(testName ${libName}-test)

# ESP32 test executable
add_executable(${testName}-esp32 
    Adafruit_SSD1306Test.cpp 
    EEPROMTest.cpp
    ESPTest.cpp 
    FSTest.cpp
    freeRTOSTest.cpp
    FSTest.cpp
    HttpClientTest.cpp 
    HTTPUpdateTest.cpp 
    IPAddressTest.cpp 
    PreferencesTest.cpp
    PubSubClientTest.cpp
    ringbufTest.cpp
    StringArduinoTest.cpp
    WiFiClientSecureTest.cpp
    WiFiTest.cpp
    WireTest.cpp
)

target_compile_definitions(${testName}-esp32 PRIVATE ARDUINO_ARCH_ESP32)
# target_sources (${testName} PRIVATE ${mySources})
target_include_directories(${libName}-esp32 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}  )
target_link_libraries(${testName}-esp32 ${libName}-esp32 gtest_main)
enable_testing()

add_test(NAME ${testName}-esp32 COMMAND ${testName}-esp32)

# ESP8266 test executable
add_executable(${testName}-esp8266
    WiFi8266Test.cpp 
    ESP8266httpUpdateTest.cpp 
)

target_compile_definitions(${testName}-esp8266 PRIVATE ARDUINO_ARCH_ESP8266)
target_include_directories(${libName}-esp8266 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}  )
target_link_libraries(${testName}-esp8266 PRIVATE ${libName}-esp8266 gtest_main)

add_test(NAME ${testName}-esp8266 COMMAND ${testName}-esp8266)