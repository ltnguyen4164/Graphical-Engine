cmake_minimum_required(VERSION 3.28)

project(ENGINE LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Vulkan REQUIRED)

# Engine
add_executable(${PROJECT_NAME}
    main.cpp
    src/sirius_r.cpp
    src/instance.cpp
    src/device.cpp
    src/surface.cpp
    src/swapchain.cpp
    src/imageview.cpp
    src/graphics.cpp
    src/commandpool.cpp
    src/synchobj.cpp
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        "${CMAKE_SOURCE_DIR}/include"
        "${Vulkan_INCLUDE_DIRS}"
)

# Vulkan-Hpp configuration
target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
        VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
)

target_link_directories(${PROJECT_NAME}
    PRIVATE
        "${CMAKE_SOURCE_DIR}/lib"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Vulkan::Vulkan
        glfw3
)

if(WIN32)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE
            gdi32
            user32
            shell32
    )
endif()