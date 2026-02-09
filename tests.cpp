//
// Created by Riccardo on 09/02/2026.
//

#include <gtest/gtest.h>
#include <stdexcept>
#include "Note.h"
#include "Collezioni.h"
#include "Observer.h"

// --- MOCK OBSERVER ---
// Serve per testare se le collezioni notificano correttamente i cambiamenti
class MockObserver : public Observer {
public:
    int lastCount = -1;
    std::string lastCollectionName = "";

    void update(int count, const std::string& collectionName) override {
        lastCount = count;
        lastCollectionName = collectionName;
    }
};

// --- TEST UNITARI ---

// 1. Verifica il funzionamento base e il blocco delle note
TEST(NoteTest, LockAndProperties) {
    Note n("Titolo", "Contenuto");
    n.setLocked(true);

    // Deve lanciare eccezione se bloccata
    EXPECT_THROW(n.setTitle("Nuovo"), std::runtime_error);

    n.setLocked(false);
    EXPECT_NO_THROW(n.setTitle("Nuovo Titolo"));
    EXPECT_EQ(n.getTitle(), "Nuovo Titolo");
}

// 2. Verifica l'esclusività tra due collezioni NORMALI
TEST(CollectionTest, NormalCollectionExclusivity) {
    Collezioni lavoro("Lavoro");
    Collezioni casa("Casa");
    Note n("Nota Test", "...");

    lavoro.addNote(&n);
    EXPECT_EQ(lavoro.getNoteCount(), 1);
    EXPECT_EQ(n.getCollezione(), &lavoro);

    // Aggiungendo a 'casa', deve essere rimossa automaticamente da 'lavoro'
    casa.addNote(&n);
    EXPECT_EQ(lavoro.getNoteCount(), 0);
    EXPECT_EQ(casa.getNoteCount(), 1);
    EXPECT_EQ(n.getCollezione(), &casa);
}

// 3. Verifica la doppia appartenenza (Normale + Importanti)
TEST(CollectionTest, DualAppartenanceImportant) {
    Collezioni lavoro("Lavoro");
    Collezioni& imp = Collezioni::getImportanti();
    Note n("Nota Urgente", "...");

    lavoro.addNote(&n);
    imp.addNote(&n);

    // Deve essere presente in ENTRAMBE
    EXPECT_EQ(lavoro.getNoteCount(), 1);
    EXPECT_EQ(imp.getNoteCount(), 1);

    // Lo stato della nota deve riflettere entrambi
    EXPECT_EQ(n.getCollezione(), &lavoro);
    EXPECT_TRUE(n.isImportante());
}

// 4. Verifica il funzionamento del Singleton per la collezione speciale
TEST(CollectionTest, SingletonSpecialCollection) {
    Collezioni& instance1 = Collezioni::getImportanti();
    Collezioni& instance2 = Collezioni::getImportanti();

    EXPECT_EQ(&instance1, &instance2); // Stesso indirizzo di memoria
    EXPECT_TRUE(instance1.isImportantiCollection());
}

// 5. Verifica il Pattern Observer
TEST(ObserverTest, NotificationOnAddRemove) {
    Collezioni testColl("Test");
    MockObserver spy;
    testColl.addObserver(&spy);

    Note n1("N1", "...");
    testColl.addNote(&n1);

    EXPECT_EQ(spy.lastCount, 1);
    EXPECT_EQ(spy.lastCollectionName, "Test");

    testColl.removeNote(&n1);
    EXPECT_EQ(spy.lastCount, 0);
}

// 6. Verifica che non si possa rimuovere una nota bloccata
TEST(CollectionTest, DenyRemovalIfLocked) {
    Collezioni coll("Sicura");
    Note n("Privato", "...");
    coll.addNote(&n);
    n.setLocked(true);

    EXPECT_THROW(coll.removeNote(&n), std::runtime_error);
    EXPECT_EQ(coll.getNoteCount(), 1);
}

// 7. Verifica che uscendo da Importanti la nota resti nella collezione normale
TEST(CollectionTest, LeaveImportantStayNormal) {
    Collezioni lavoro("Lavoro");
    Collezioni& imp = Collezioni::getImportanti();
    Note n("Task", "...");

    lavoro.addNote(&n);
    imp.addNote(&n);

    // Rimuovo da importanti
    imp.removeNote(&n);

    EXPECT_FALSE(n.isImportante());
    EXPECT_EQ(lavoro.getNoteCount(), 1); // Resta in lavoro
    EXPECT_EQ(n.getCollezione(), &lavoro);
}