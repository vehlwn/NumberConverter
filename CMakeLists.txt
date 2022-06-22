cmake_minimum_required(VERSION 3.5)

project(NumberConverter LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt5 REQUIRED COMPONENTS 
    Widgets
)

find_package(Boost REQUIRED)

set(GUI_EXECUTABLE)
if(WIN32)
	set(GUI_EXECUTABLE WIN32)
elseif(APPLE)
	set(GUI_EXECUTABLE MACOSX_BUNDLE)
endif()


add_executable(
    ${PROJECT_NAME} ${GUI_EXECUTABLE}
    main.cpp
    MainWindow.cpp
    MainWindow.h
    MainWindow.ui
    NumberConverter.cpp
    NumberConverter.h
)
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
	${Boost_INCLUDE_DIRS}
)
target_link_libraries(${PROJECT_NAME} PRIVATE 
    Qt5::Widgets
)
