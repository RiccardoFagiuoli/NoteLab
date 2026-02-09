//
// Created by Riccardo on 09/02/2026.
//

#ifndef NOTELAB_COLLEZIONI_H
#define NOTELAB_COLLEZIONI_H

#include "Note.h"
#include "Observer.h"
#include <vector>
#include <string>
#include <algorithm>

class Collection {
    public:
        Collection(std::string name);

        void addNote(Note* note);
        void removeNote(Note* note);

        void addObserver(Observer* obs);
        void removeObserver(Observer* obs);
        void notify();

        int getNoteCount() const { return notes.size(); }
        std::string getName() const { return name; }

    private:
        std::string name;
        std::vector<Note*> notes;
        std::vector<Observer*> observers;
};

#endif //NOTELAB_COLLEZIONI_H