cmake_minimum_required(VERSION 3.6)
project (curlsession)

if(NOT TARGET jsonserializer)
    set(JSON_FILE ${PROJECT_SOURCE_DIR}/serializer.json)
    set(GENERATED_DIRECTORY ${PROJECT_SOURCE_DIR}/generated)
    set(GENERATED_HEADER ${PROJECT_SOURCE_DIR}/generated.h)
    add_subdirectory(cpp-jsonserializer)
endif()

include(FindCURL)

set(SOURCES 
${PROJECT_SOURCE_DIR}/src/CurlSession.cpp 
${PROJECT_SOURCE_DIR}/src/RequestParams.cpp
${PROJECT_SOURCE_DIR}/src/entities.cpp
)

include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR})
include_directories(${OPENSSL_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS})
link_directories(${OPENSSL_LIBRARIES} ${CURL_LIBRARIES})

add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_include_directories (${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC "curl" "jsonserializer")
