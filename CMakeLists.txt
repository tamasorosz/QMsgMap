cmake_minimum_required(VERSION 3.20)
project(QMsgMap LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)
find_package(Boost REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(RABBITMQ REQUIRED IMPORTED_TARGET librabbitmq)

include(FetchContent)
FetchContent_Declare(
  SimpleAmqpClient
  GIT_REPOSITORY https://github.com/alanxz/SimpleAmqpClient.git
  GIT_TAG        v2.6.0   # ha más kell, átírod
)
FetchContent_MakeAvailable(SimpleAmqpClient)

add_executable(qmsgmap
  src/main.cpp
  # ... a fájljaid
)

target_link_libraries(qmsgmap PRIVATE
  Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick
  Boost::boost
  PkgConfig::RABBITMQ
  SimpleAmqpClient
)