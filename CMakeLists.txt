# This is a build file for Huggle (used with cmake)
# You will need to update it by hand!
cmake_minimum_required (VERSION 2.8.7)
PROJECT(huggle_sh)
#  cmake <folder> -DQT5_BUILD=true
option(QT5_BUILD "Build huggle using the Qt5 framework" FALSE)
SET(CMAKE_CXX_FLAGS "-std=c++11")
SET(QT_USE_QTNETWORK TRUE)
SET(QT_USE_QTXML TRUE)
SET(CMAKE_BUILD_TYPE Release)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_AUTOMOC ON)
include_directories(../../)
#### Qt 4 and 5 ####
if(QT5_BUILD)
    find_package(Qt5Core REQUIRED)
    find_package(Qt5Gui REQUIRED)
    find_package(Qt5Xml REQUIRED)
    find_package(Qt5Widgets REQUIRED)
    find_package(Qt5Network REQUIRED)
    set(QT_INCLUDES
        ${Qt5Gui_INCLUDE_DIRS}
        ${Qt5Widgets_INCLUDE_DIRS}
        ${Qt5Network_INCLUDE_DIRS}
        ${Qt5Xml_INCLUDE_DIRS}
        ${Qt5Svg_INCLUDE_DIRS}
    )
    include_directories(${QT_INCLUDES})
else()
    find_package(Qt4 REQUIRED)
endif()

file(GLOB sh_srcx
    "src/*.cpp"
)
file (GLOB sh_resx
    "src/*.qrc"
)
file (GLOB sh_uixx
    "src/*.ui"
)

SET(sh_SOURCES ${sh_srcx})
SET(sh_FORMS ${sh_uixx} "../../mainwindow.ui")
SET(sh_RESOURCES ${sh_resx})
if(QT5_BUILD)
    QT5_WRAP_CPP(sh_HEADERS_MOC ${sh_HEADERS})
    QT5_WRAP_UI(sh_FORMS_HEADERS ${sh_FORMS})
    QT5_ADD_RESOURCES(sh_RESOURCES_RCC ${sh_RESOURCES})
else()
    QT4_WRAP_CPP(sh_HEADERS_MOC ${sh_HEADERS})
    QT4_WRAP_UI(sh_FORMS_HEADERS ${sh_FORMS})
    QT4_ADD_RESOURCES(sh_RESOURCES_RCC ${sh_RESOURCES})
endif()
if (NOT QT5_BUILD)
  INCLUDE(${QT_USE_FILE})
endif()
ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS( -DQT_USE_QSTRINGBUILDER )
ADD_LIBRARY(huggle_sh SHARED ${sh_SOURCES} ${sh_HEADERS_MOC} ${sh_FORMS_HEADERS} ${sh_RESOURCES_RCC})
TARGET_LINK_LIBRARIES(huggle_sh ${QT_LIBRARIES})
if (NOT "${CMAKE_SYSTEM}" MATCHES "Linux")
    TARGET_LINK_LIBRARIES(huggle_sh core)
endif ()
if (QT5_BUILD)
    qt5_use_modules(huggle_sh Core Gui Widgets Xml)
endif()
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
if (LINUX_SNAP)
    INSTALL(TARGETS huggle_sh DESTINATION bin/extensions)
else()
    INSTALL(TARGETS huggle_sh DESTINATION share/huggle/extensions)
endif()
