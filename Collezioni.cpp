//
// Created by Riccardo on 09/02/2026.
//
#include "Collezioni.h"

Collection::Collection(std::string name) : name(name) {}

void Collection::addNote(Note* note) {
    notes.push_back(note);
    notify();
}

void Collection::removeNote(Note* note) {
    if (note->isLocked()) throw std::runtime_error("Impossibile rimuovere una nota bloccata.");

    auto it = std::find(notes.begin(), notes.end(), note);
    if (it != notes.end()) {
        notes.erase(it);
        notify();
    }
}

void Collection::addObserver(Observer* obs) {
    observers.push_back(obs);
}

void Collection::removeObserver(Observer* obs) {
    observers.erase(std::remove(observers.begin(), observers.end(), obs), observers.end());
}

void Collection::notify() {
    for (auto obs : observers) {
        obs->update(notes.size(), name);
    }
}