# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/Hiep/esp32/Espressif/frameworks/esp-idf-v5.2.3/components/bootloader/subproject"
  "D:/Hiep/esp32/VXL_AI_DLDT/Line-FolloweRobot/build/bootloader"
  "D:/Hiep/esp32/VXL_AI_DLDT/Line-FolloweRobot/build/bootloader-prefix"
  "D:/Hiep/esp32/VXL_AI_DLDT/Line-FolloweRobot/build/bootloader-prefix/tmp"
  "D:/Hiep/esp32/VXL_AI_DLDT/Line-FolloweRobot/build/bootloader-prefix/src/bootloader-stamp"
  "D:/Hiep/esp32/VXL_AI_DLDT/Line-FolloweRobot/build/bootloader-prefix/src"
  "D:/Hiep/esp32/VXL_AI_DLDT/Line-FolloweRobot/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/Hiep/esp32/VXL_AI_DLDT/Line-FolloweRobot/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/Hiep/esp32/VXL_AI_DLDT/Line-FolloweRobot/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
