//
// Created by Riccardo on 09/02/2026.
//

#ifndef NOTELAB_NOTE_H
#define NOTELAB_NOTE_H

#include <string>

class Collezioni;

class Note {
    public:
        Note(std::string title, std::string text);
        ~Note();

        void setTitle(const std::string& newTitle);
        void setText(const std::string& newText);

        std::string getTitle() const { return title; }
        std::string getText() const { return text; }

        void setLocked(bool lock) { locked = lock; }
        bool isLocked() const { return locked; }

        void setCollezione(Collezioni* coll);
        Collezioni* getCollezione() const { return collezione; }

        void setImportante(bool imp);
        bool isImportante() const { return importante; }

    private:
        std::string title;
        std::string text;
        bool locked = false;
        bool importante = false;
        Collezioni* collezione = nullptr;
};

#endif //NOTELAB_NOTE_H