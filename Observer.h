//
// Created by Riccardo on 09/02/2026.
//

#ifndef NOTELAB_OBSERVER_H
#define NOTELAB_OBSERVER_H

class Observer {
    public:
        virtual ~Observer() = default;
        virtual void update(int count, const std::string& collectionName) = 0;
};

#endif //NOTELAB_OBSERVER_H