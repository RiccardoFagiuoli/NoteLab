//
// Created by Riccardo on 09/02/2026.
//
#include "Collezioni.h"
#include <algorithm>
#include <stdexcept>

Collezioni::Collezioni(std::string name, bool importante)
    : name(name), isSpecial(importante) {}

// Implementazione Singleton per la collezione speciale
Collezioni& Collezioni::getImportanti() {
    static Collezioni instance("Importanti", true);
    return instance;
}

void Collezioni::addNote(Note* note) {
    if (!note) return;

    if (this->isSpecial) {
        // --- CASO COLLEZIONE SPECIALI ---
        // Non serve rimuoverla dalle altre perché la nota può stare in entrambe.
        auto it = std::find(notes.begin(), notes.end(), note);
        if (it == notes.end()) {
            notes.push_back(note);
            note->setImportante(true);
            notify();
        }
    } else {
        // --- CASO COLLEZIONE NORMALE ---
        // Se la nota è già in un'ALTRA collezione normale, la rimuoviamo da lì
        if (note->getCollezione() != nullptr && note->getCollezione() != this) {
            note->getCollezione()->removeNote(note);
        }

        // Se non è già in questa collezione, la aggiungiamo
        auto it = std::find(notes.begin(), notes.end(), note);
        if (it == notes.end()) {
            notes.push_back(note);
            note->setCollezione(this);
            notify();
        }
    }
}

void Collezioni::removeNote(Note* note) {
    if (note->isLocked() && !this->isSpecial) throw std::runtime_error("Nota bloccata.");

    auto it = std::find(notes.begin(), notes.end(), note);
    if (it != notes.end()) {
        notes.erase(it);

        if (this->isSpecial) {
            note->setImportante(false);
        } else {
            note->setCollezione(nullptr);
        }
        notify();
    }
}

void Collezioni::destructorRemove(Note* note) {
    if (note->isLocked()) throw std::runtime_error("Nota bloccata.");
    auto it = std::find(notes.begin(), notes.end(), note);
    if (it != notes.end()) {
        notes.erase(it);

        if (this->isSpecial) {
            note->setImportante(false);
        } else {
            note->setCollezione(nullptr);
        }
        notify();
    }
}

void Collezioni::notify() {
    for (auto obs : observers) obs->update(notes.size(), name);
}