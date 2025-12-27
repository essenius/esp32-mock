# esp32-mock

Mock implementation of common ESP32 functions so applications can be tested without a physical device.

Not everything is implemented - just the functions I needed so far for my own projects.

It can be used using vcpkg with the [vcpkg registry in this account](../../../vcpkg), port esp32-mock.

The following SonarQube rules should be ignored/disabled as we are mimicking an existing interface:

  - cpp:S5028 ("replace macro by const, constexpr or enum")
  - cpp:S5817 ("method could be const")
