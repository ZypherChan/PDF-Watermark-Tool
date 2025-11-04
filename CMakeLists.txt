cmake_minimum_required(VERSION 3.10)
project(PdfTool)

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "only support x64")
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(MSVC)	
	# disable warning C4267: conversion from 'size_t' to 'type', possible loss of data
	# disable warning C4100: unreferenced formal parameter
	# disable warning C4244: conversion from 'type1' to 'type2', possible loss of data 
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4267 /wd4100 /wd4244")
	set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /wd4267 /wd4100 /wd4244")
		
	add_definitions(-D_UNICODE -DUNICODE -DWIN32 -D_CRT_SECURE_NO_WARNINGS)
endif(MSVC)

file(GLOB SFNTLY_CORE_FILES ttfsubset/sfntly/*.h ttfsubset/sfntly/*.cc)
file(GLOB SFNTLY_PORT_FILES ttfsubset/sfntly/port/*.h ttfsubset/sfntly/port/*.cc)
file(GLOB SFNTLY_DATA_FILES ttfsubset/sfntly/data/*.h ttfsubset/sfntly/data/*.cc)
file(GLOB SFNTLY_MATH_FILES ttfsubset/sfntly/math/*.h ttfsubset/sfntly/math/*.cc)
file(GLOB SFNTLY_TABLE_COMMON_FILES ttfsubset/sfntly/table/*.h ttfsubset/sfntly/table/*.cc)
file(GLOB SFNTLY_TABLE_BITMAP_FILES ttfsubset/sfntly/table/bitmap/*.h ttfsubset/sfntly/table/bitmap/*.cc)
file(GLOB SFNTLY_TABLE_CORE_FILES ttfsubset/sfntly/table/core/*.h ttfsubset/sfntly/table/core/*.cc)
file(GLOB SFNTLY_TABLE_TTF_FILES ttfsubset/sfntly/table/truetype/*.h ttfsubset/sfntly/table/truetype/*.cc)
file(GLOB SUBTLY_CORE_FILES ttfsubset/subtly/*.h ttfsubset/subtly/*.cc)
file(GLOB SUBTTF_CORE_FILES ttfsubset/*.h ttfsubset/*.cpp)

# vs project file filter tree
source_group(sfntly\\core FILES ${SFNTLY_CORE_FILES})
source_group(sfntly\\ports FILES ${SFNTLY_PORT_FILES})
source_group(sfntly\\data FILES ${SFNTLY_DATA_FILES})
source_group(sfntly\\math FILES ${SFNTLY_MATH_FILES})
source_group(sfntly\\table FILES ${SFNTLY_TABLE_COMMON_FILES})
source_group(sfntly\\table\\bitmap FILES ${SFNTLY_TABLE_BITMAP_FILES})
source_group(sfntly\\table\\core FILES ${SFNTLY_TABLE_CORE_FILES})
source_group(sfntly\\table\\truetype FILES ${SFNTLY_TABLE_TTF_FILES})
source_group(subtly\\core FILES ${SUBTLY_CORE_FILES})
source_group(subttf FILES ${SUBTTF_CORE_FILES})
			  
file(GLOB sources *.cpp)
file(GLOB headers *.h)
file(GLOB resources *.qrc *.rc)
file(GLOB forms *.ui)

source_group("Form Files" FILES ${forms})
source_group("Header Files" FILES ${headers})
source_group("Resource Files" FILES ${resources})
source_group("Source Files" FILES ${sources})
			  
include_directories(${PROJECT_NAME} 
					${CMAKE_CURRENT_SOURCE_DIR}
					${CMAKE_CURRENT_SOURCE_DIR}/ttfsubset
					${CMAKE_CURRENT_SOURCE_DIR}/ttfsubset/sfntly
					${CMAKE_CURRENT_SOURCE_DIR}/ttfsubset/subtly)
							
add_definitions(-DSUBTTF_EXPORTS -DSFNTLY_NO_EXCEPTION -DSFNTLY_EXPERIMENTAL)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(${PROJECT_NAME} 
              ${SFNTLY_CORE_FILES}
              ${SFNTLY_PORT_FILES}
              ${SFNTLY_DATA_FILES}
              ${SFNTLY_MATH_FILES}
              ${SFNTLY_TABLE_COMMON_FILES}
              ${SFNTLY_TABLE_BITMAP_FILES}
              ${SFNTLY_TABLE_CORE_FILES}
              ${SFNTLY_TABLE_TTF_FILES}
			  ${SUBTLY_CORE_FILES}
			  ${SUBTTF_CORE_FILES}
			  ${sources}
			  ${headers} 
			  ${resources} 
			  ${forms})

target_link_directories(${PROJECT_NAME} PRIVATE pdfium/lib)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Gui Qt5::Widgets pdfium.dll.lib)
if(WIN32)
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif(WIN32)

find_program(QMAKE_EXECUTABLE NAMES qmake HINTS ${QTDIR} ENV QTDIR PATH_SUFFIXES bin)
execute_process(COMMAND ${QMAKE_EXECUTABLE} -query QT_VERSION OUTPUT_VARIABLE QT_VERSION)
message("QT version is ${QT_VERSION}")

find_program(WINDEPLOYQT_EXECUTABLE NAMES windeployqt windeployqt.exe 
    HINTS 
        $ENV{QTDIR}
        ${QTDIR}
        ${Qt5_DIR}/../../../bin
    PATH_SUFFIXES bin
    DOC "Path to windeployqt executable"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/fonts"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/fonts"
    COMMENT "Copying fonts directory to output directory"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND "${WINDEPLOYQT_EXECUTABLE}" 
    "$<TARGET_FILE:${PROJECT_NAME}>"
    --no-translations
    --no-system-d3d-compiler
    --no-opengl-sw
    --no-angle
    --no-webkit2
    --no-quick-import
    --no-qmltooling
    COMMENT "Running windeployqt to deploy Qt dependencies"
)

if(NOT EXISTS "${WINDEPLOYQT_EXECUTABLE}")
    message(WARNING "windeployqt.exe not found!")
    message(WARNING "Searched locations:")
    message(WARNING "  - $ENV{QTDIR}/bin")
    message(WARNING "  - ${QTDIR}/bin") 
    message(WARNING "  - ${Qt5_DIR}/../../../bin")
    message(WARNING "Qt dependencies will not be automatically deployed!")
    message(WARNING "Please ensure Qt5 bin directory is in your PATH or set QTDIR environment variable")
else()
    message(STATUS "Found windeployqt: ${WINDEPLOYQT_EXECUTABLE}")
endif()

