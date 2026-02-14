//
// Created by Riccardo on 14/02/2026.
//

#ifndef NOTELAB_NOTECOUNTER_H
#define NOTELAB_NOTECOUNTER_H

#include <iostream>
#include "Observer.h"

// Questo è l'Observer concreto
class NoteCounter : public Observer {
    public:
        void update(int count, const std::string& collectionName) override {
            std::cout << "[NOTIFICA] La collezione '" << collectionName
                      << "' ora ha " << count << " note." << std::endl;
        }
};

#endif //NOTELAB_NOTECOUNTER_H