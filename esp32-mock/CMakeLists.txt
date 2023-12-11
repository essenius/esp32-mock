include(FindGit)
find_package(Git)
include(FetchContent)

FetchContent_Declare(
  safe-cstring
  GIT_REPOSITORY https://github.com/essenius/safe-cstring
  GIT_TAG        main
}

FetchContent_MakeAvailable(safe-cstring)

add_library(${libName} "")

set(myHeaders Adafruit_SSD1306.h Client.h ESP.h HTTPClient.h HTTPUpdate.h IPAddress.h Preferences.h PubSubClient.h StringArduino.h WiFi.h WiFiClient.h WiFiClientSecure.h Wire.h freertos/freeRTOS.h freertos/ringbuf.h freertos/semphr.h sys/time/h)
set(mySources Client.cpp ESP.cpp HTTPClient.cpp HTTPUpdate.cpp IPAddress.cpp Preferences.cpp PubSubClient.cpp WiFi.cpp WiFiClientSecure.cpp Wire.cpp freertos/freeRTOS.cpp freertos/ringbuf.cpp sys/time.cpp)

target_sources (${libName} PUBLIC ${myHeaders} PRIVATE ${mySources})
target_include_directories(${libName} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS ${libName} DESTINATION lib)
install(FILES ${myHeaders} DESTINATION include) 