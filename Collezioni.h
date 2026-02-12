//
// Created by Riccardo on 09/02/2026.
//

#ifndef NOTELAB_COLLEZIONI_H
#define NOTELAB_COLLEZIONI_H

#include <vector>
#include <string>
#include "Note.h"
#include "Observer.h"

class Collezioni {
    public:
        Collezioni(std::string name, bool isSpecialImportant = false);

        // Metodo statico per ottenere la collezione speciale unica
        static Collezioni& getImportanti();

        void addNote(Note* note);
        void removeNote(Note* note);

        void addObserver(Observer* obs) { observers.push_back(obs); }
        void notify();

        int getNoteCount() const { return notes.size(); }
        std::string getName() const { return name; }
        bool getIsSpecial() const { return isSpecial; }

        void clear() { notes.clear(); }
        // Metodo per rimuovere una nota senza notificare (usato dal distruttore di Note)
        void destructorRemove(Note* note);

        const std::vector<Note*>& getNote() const {
            return notes; // Sostituisci 'note' con il nome del tuo vector
        }

    private:
        std::string name;
        std::vector<Note*> notes;
        std::vector<Observer*> observers;
        bool isSpecial; // Indica se è la collezione "Importanti"
};

#endif //NOTELAB_COLLEZIONI_H