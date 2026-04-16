cmake_minimum_required(VERSION 3.16)

project(WeatherAI VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Szukamy wszystkich potrzebnych modułów Qt
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network Test)

# 2. Definiujemy źródła TYLKO dla głównej aplikacji
# USUNĄŁEM stąd tests/test_logic.cpp
set(APP_SOURCES
    main.cpp
    src/api/ollamaclient.cpp
    src/api/ollamaclient.h
    src/gui/mainwindow.cpp
    src/gui/mainwindow.h
    src/gui/mainwindow.ui
    src/logic/scriptexecutor.cpp
    src/logic/scriptexecutor.h
)

# 3. Informujemy CMake, gdzie szukać nagłówków
include_directories(src/api src/gui src/logic)

# 4. Tworzymy główny program WeatherAI
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(WeatherAI
        MANUAL_FINALIZATION
        ${APP_SOURCES}
    )
else()
    if(ANDROID)
        add_library(WeatherAI SHARED ${APP_SOURCES})
    else()
        add_executable(WeatherAI ${APP_SOURCES})
    endif()
endif()

# 5. Linkujemy biblioteki dla głównej aplikacji
target_link_libraries(WeatherAI PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
)

# --- Ustawienia systemowe (macOS/Windows) ---
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.WeatherAI)
endif()

set_target_properties(WeatherAI PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# --- Konfiguracja testów - OSOBNY CEL (WeatherTests) ---
enable_testing()

add_executable(WeatherTests
    tests/test_logic.cpp
    src/logic/scriptexecutor.cpp # Dodajemy plik, który testujemy
)

# Testy linkujemy osobno z modułem Test
target_link_libraries(WeatherTests PRIVATE
    Qt${QT_VERSION_MAJOR}::Test
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
)

# Dodatkowe ścieżki dla testów
target_include_directories(WeatherTests PRIVATE src/logic)

# Finalizacja dla Qt6
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(WeatherAI)
endif()