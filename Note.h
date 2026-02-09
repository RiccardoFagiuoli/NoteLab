//
// Created by Riccardo on 09/02/2026.
//

#ifndef NOTELAB_NOTE_H
#define NOTELAB_NOTE_H

#include <string>
#include <stdexcept>

class Note {
    public:
        Note(std::string title, std::string text);

        void setTitle(const std::string& newTitle);
        void setText(const std::string& newText);

        std::string getTitle() const { return title; }
        std::string getText() const { return text; }

        void setLocked(bool lock) { locked = lock; }
        bool isLocked() const { return locked; }

        void setImportant(bool imp) { important = imp; }
        bool isImportant() const { return important; }

    private:
        std::string title;
        std::string text;
        bool locked;
        bool important;
};

#endif //NOTELAB_NOTE_H