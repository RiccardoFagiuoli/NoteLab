cmake_minimum_required(VERSION 4.0)
project(NoteLab)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_PREFIX_PATH "C:/Programmi/Qt/6.9.1/mingw_64")
# 1. Queste tre devono essere ON
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 2. Cerca le librerie
find_package(Qt6 REQUIRED COMPONENTS Widgets)


# Forza l'uso delle librerie dinamiche per evitare errori di link su Windows (MSVC)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# 1. Aggiungi la sottocartella googletest
add_subdirectory(googletest)

# 2. Specifica dove trovare gli header di gtest
include_directories(googletest/googletest/include googletest/googlemock/include)

# 3. Definisci la libreria "Core" (La logica del tuo programma)
# Qui mettiamo i file che servono sia al programma vero che ai test.
# Ho corretto il nome in Collezioni.cpp per coerenza con il tuo add_executable
add_library(NoteCore
Note.cpp
        Note.h
        Collezioni.h
Collezioni.cpp)

# 4. Crea l'eseguibile del programma principale (l'App vera e propria)
# NON includere tests.cpp qui, altrimenti avrai due funzioni main() in conflitto!
add_executable(NoteLab main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui)
target_link_libraries(NoteLab PRIVATE
        NoteCore
Qt6::Widgets)

# 5. Crea l'eseguibile per i Test
# Qui colleghiamo i test alla logica (NoteCore) e a GoogleTest
add_executable(run_tests tests.cpp)
target_link_libraries(run_tests PRIVATE NoteCore gtest gtest_main)

# 8. Assicuriamoci che tutti trovino gli header nella cartella principale
target_include_directories(NoteCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(NoteLab PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(run_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

qt_finalize_executable(NoteLab)