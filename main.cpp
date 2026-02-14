#include <iostream>
#include "Collezioni.h"
#include "Note.h"
#include "NoteCounter.h"

int main() {
    NoteCounter counter;

    Collezioni& imp = Collezioni::getImportanti();
    Collezioni lavoro("Lavoro");
    Collezioni casa("Casa");

    imp.addObserver(&counter);
    lavoro.addObserver(&counter);
    casa.addObserver(&counter);

    std::cout << "--- Inizio Test ---" << std::endl;
    Note* n = new Note("Lista Spesa", "...");

    lavoro.addNote(n);

    imp.addNote(n);

    std::cout << "\nStato attuale:" << std::endl;
    std::cout << "In Importanti: " << (n->isImportante() ? "Si" : "No") << std::endl;
    if (n->getCollezione())
        std::cout << "Collezione: " << n->getCollezione()->getName() << std::endl;


    std::cout << "\n--- Spostamento in Casa ---" << std::endl;
    casa.addNote(n);

    std::cout << "\nDopo spostamento:" << std::endl;
    std::cout << "Count Lavoro: " << lavoro.getNoteCount() << std::endl;
    std::cout << "Count Casa: " << casa.getNoteCount() << std::endl;

    std::cout << "\n--- Eliminazione nota ---" << std::endl;
    delete n;

    std::cout << "\nFine test. Count finale Importanti: " << imp.getNoteCount() << std::endl;

    return 0;
}