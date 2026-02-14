//
// Created by Riccardo on 09/02/2026.
//

#ifndef NOTELAB_COLLEZIONI_H
#define NOTELAB_COLLEZIONI_H

#include <vector>
#include <string>
#include "Note.h"
#include "Observer.h"
#include <algorithm>

class Collezioni {
    public:
        Collezioni(std::string name, bool isSpecialImportant = false);

        // Metodo statico per ottenere la collezione speciale unica
        static Collezioni& getImportanti();

        void addNote(Note* note);
        void removeNote(Note* note);

        void addObserver(Observer* obs) { observers.push_back(obs); }
        void removeObserver(Observer* obs) {
            observers.erase(std::remove(observers.begin(), observers.end(), obs), observers.end());
        }
        void notify();

        int getNoteCount() const { return notes.size(); }
        std::string getName() const { return name; }
        bool isImportantiCollection() const { return isSpecial; }

        void clear() { notes.clear(); }
        // Metodo per rimuovere una nota senza notificare (usato dal distruttore di Note)
        void destructorRemove(Note* note);

    private:
        std::string name;
        std::vector<Note*> notes;
        std::vector<Observer*> observers;
        bool isSpecial; // Indica se è la collezione "Importanti"
};

#endif //NOTELAB_COLLEZIONI_H