//
// Created by Riccardo on 09/02/2026.
//
#include "Note.h"

Note::Note(std::string title, std::string text)
    : title(title), text(text), locked(false), important(false) {}

void Note::setTitle(const std::string& newTitle) {
    if (locked) throw std::runtime_error("Nota bloccata: modifica non permessa.");
    title = newTitle;
}

void Note::setText(const std::string& newText) {
    if (locked) throw std::runtime_error("Nota bloccata: modifica non permessa.");
    text = newText;
}