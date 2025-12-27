target_sources(${libName}-esp32 PUBLIC freeRTOS.h ringbuf.h semphr.h PRIVATE freeRTOS.cpp ringbuf.cpp)
target_sources(${libName}-esp8266 PUBLIC freeRTOS.h ringbuf.h semphr.h PRIVATE freeRTOS.cpp ringbuf.cpp)
install (FILES freeRTOS.h ringbuf.h semphr.h DESTINATION include/freertos)