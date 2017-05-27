# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html




# todo 1. 设置 CMake 版本。
# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)





# todo 2. 创建本地库，添加本地库: 名称、类型、 相对路径。
# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library(
             # todo 库名称(与文件名无关)，loadLibrary时使用此名称，而非文件名。
             # Sets the name of the library.
             libName

             # Sets the library as a shared library.
             SHARED

             # todo 对应 文件名
             # Provides a relative path to your source file(s).
             src/main/cpp/native-lib.cpp
             src/main/cpp/test_tony.c
             src/main/cpp/p2pclient.c
             )



# 导入cpp目录下的所有头文件
#include_directories(src/main/cpp/)




# todo 3. 查找预编译的本地库 ？？？
# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that you want CMake to locate.
              log )





# todo 4. 链接多个本地库 ？？？
# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries(
                       # todo 对应 库名称
                       # Specifies the target library.
                       libName

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )