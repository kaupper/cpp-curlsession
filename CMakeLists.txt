cmake_minimum_required(VERSION 3.6)
project (curlsession)

include(FindCURL)

set(SOURCES 
    ${PROJECT_SOURCE_DIR}/src/CurlSession.cpp 
    ${PROJECT_SOURCE_DIR}/src/RequestBuilder.cpp
    ${PROJECT_SOURCE_DIR}/src/Commons.cpp
    ${PROJECT_SOURCE_DIR}/src/entities.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_include_directories (${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${OPENSSL_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include/curlsession)
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC "curl")
