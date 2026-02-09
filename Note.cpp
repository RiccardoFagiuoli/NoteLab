//
// Created by Riccardo on 09/02/2026.
//
#include "Note.h"
#include "Collezioni.h"
#include <stdexcept>

Note::Note(std::string title, std::string text) : title(title), text(text) {}

void Note::setTitle(const std::string& newTitle) {
    if (locked) throw std::runtime_error("Bloccata");
    title = newTitle;
}

void Note::setText(const std::string& newText) {
    if (locked) throw std::runtime_error("Bloccata");
    text = newText;
}

void Note::setCollezione(Collezioni* coll) {
    collezione = coll;
}

void Note::setImportante(bool imp) {
    importante = imp;
}