#include <iostream>
#include "Collezioni.h"
#include "Note.h"

int main() {
    Collezioni& imp = Collezioni::getImportanti();
    Collezioni lavoro("Lavoro");
    Collezioni casa("Casa");

    Note* n = new Note("Lista", "...");

    lavoro.addNote(n);
    imp.addNote(n);

    std::cout << "In Importanti: " << (n->isImportante() ? "Si" : "No") << std::endl;
    std::cout << "Collezione: " << n->getCollezione()->getName() << std::endl;

    casa.addNote(n);

    std::cout << "\nDopo spostamento in Casa:" << std::endl;
    std::cout << "In Importanti: " << (n->isImportante() ? "Si" : "No") << std::endl;
    std::cout << "Collezione: " << n->getCollezione()->getName() << std::endl;
    std::cout << "Count Lavoro: " << lavoro.getNoteCount() << std::endl; // Sarà 0
    std::cout << "Count Casa: " << casa.getNoteCount() << std::endl; // Sarà 0

    delete n;
    std::cout << "\nDopo eliminazione nota:" << std::endl;
    std::cout << "In Importanti: " << imp.getNoteCount() << std::endl;
    std::cout << "Count Lavoro: " << lavoro.getNoteCount() << std::endl; // Sarà 0
    std::cout << "Count Casa: " << casa.getNoteCount() << std::endl; // Sarà 0

    return 0;
}