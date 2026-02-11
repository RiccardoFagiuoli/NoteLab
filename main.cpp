#include <iostream>
#include "Collezioni.h"
#include "Note.h"
#include <QApplication>
#include "mainwindow.h"


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    /*
    Collezioni& imp = Collezioni::getImportanti();
    Collezioni lavoro("Lavoro");
    Collezioni casa("Casa");

    Note* n = new Note("Lista", "...");

    lavoro.addNote(n);
    imp.addNote(n);
    // Ora è in Lavoro AND Importanti (OK!)

    std::cout << "In Importanti: " << (n->isImportante() ? "Si" : "No") << std::endl;
    std::cout << "Collezione: " << n->getCollezione()->getName() << std::endl;

    casa.addNote(n);
    // Spostata da Lavoro a Casa, ma resta in Importanti

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
    */

    return a.exec();
}