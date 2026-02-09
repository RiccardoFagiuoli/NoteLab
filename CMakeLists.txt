cmake_minimum_required(VERSION 4.0)
project(NoteLab)

set(CMAKE_CXX_STANDARD 20)

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
        Collezioni.cpp)

# 4. Crea l'eseguibile del programma principale (l'App vera e propria)
# NON includere tests.cpp qui, altrimenti avrai due funzioni main() in conflitto!
add_executable(NoteLab main.cpp)
target_link_libraries(NoteLab NoteCore)

# 5. Crea l'eseguibile per i Test
# Qui colleghiamo i test alla logica (NoteCore) e a GoogleTest
add_executable(run_tests tests.cpp)
target_link_libraries(run_tests NoteCore gtest gtest_main)