//
// Created by Riccardo on 09/02/2026.
//

#include <gtest/gtest.h>
#include <stdexcept>
#include "Note.h"
#include "Collezioni.h"
#include "Observer.h"

// --- MOCK OBSERVER ---
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

// 1. Funzionamento base e blocco modifiche
TEST(NoteTest, LockAndProperties) {
    Note n("Titolo", "Contenuto");
    n.setLocked(true);

    EXPECT_THROW(n.setTitle("Nuovo"), std::runtime_error);

    n.setLocked(false);
    EXPECT_NO_THROW(n.setTitle("Nuovo Titolo"));
    EXPECT_EQ(n.getTitle(), "Nuovo Titolo");
}

// 2. Esclusività tra collezioni NORMALI
TEST(CollectionTest, NormalCollectionExclusivity) {
    Collezioni lavoro("Lavoro");
    Collezioni casa("Casa");
    Note n("Nota Test", "...");

    lavoro.addNote(&n);
    EXPECT_EQ(lavoro.getNoteCount(), 1);
    EXPECT_EQ(n.getCollezione(), &lavoro);

    casa.addNote(&n);
    EXPECT_EQ(lavoro.getNoteCount(), 0);
    EXPECT_EQ(casa.getNoteCount(), 1);
    EXPECT_EQ(n.getCollezione(), &casa);
}

// 3. Doppia appartenenza (Normale + Importanti)
TEST(CollectionTest, DualAppartenanceImportant) {
    Collezioni lavoro("Lavoro");
    Collezioni& imp = Collezioni::getImportanti();
    Note n("Nota Urgente", "...");

    lavoro.addNote(&n);
    imp.addNote(&n);

    EXPECT_EQ(lavoro.getNoteCount(), 1);
    EXPECT_EQ(imp.getNoteCount(), 1);
    EXPECT_EQ(n.getCollezione(), &lavoro);
    EXPECT_TRUE(n.isImportante());
}

// 4. Singleton della collezione speciale
TEST(CollectionTest, SingletonSpecialCollection) {
    Collezioni& instance1 = Collezioni::getImportanti();
    Collezioni& instance2 = Collezioni::getImportanti();

    EXPECT_EQ(&instance1, &instance2);
    EXPECT_TRUE(instance1.isImportantiCollection());
}

// 5. Pattern Observer con notifiche singole
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

// 6. Blocco rimozione nota protetta
TEST(CollectionTest, DenyRemovalIfLocked) {
    Collezioni coll("Sicura");
    Note n("Privato", "...");
    coll.addNote(&n);
    n.setLocked(true);

    EXPECT_THROW(coll.removeNote(&n), std::runtime_error);
    EXPECT_EQ(coll.getNoteCount(), 1);
}

// 7. Persistenza stato normale dopo uscita da Importanti
TEST(CollectionTest, LeaveImportantStayNormal) {
    Collezioni lavoro("Lavoro");
    Collezioni& imp = Collezioni::getImportanti();
    Note n("Task", "...");

    lavoro.addNote(&n);
    imp.addNote(&n);

    imp.removeNote(&n);

    EXPECT_FALSE(n.isImportante());
    EXPECT_EQ(lavoro.getNoteCount(), 1);
    EXPECT_EQ(n.getCollezione(), &lavoro);
}

// --- NUOVE AGGIUNTE E MIGLIORAMENTI ---

// 8. Test della Nota Orfana (corretta gestione puntatori nulli)
TEST(CollectionTest, OrphanNoteManagement) {
    Collezioni studio("Studio");
    Note n("Appunti", "...");

    EXPECT_EQ(n.getCollezione(), nullptr);

    studio.addNote(&n);
    EXPECT_EQ(n.getCollezione(), &studio);

    studio.removeNote(&n);
    EXPECT_EQ(n.getCollezione(), nullptr);
}

// 9. Test Osservatori Multipli (Broadcast della notifica)
TEST(ObserverTest, MultipleObserversBroadcast) {
    Collezioni ufficio("Ufficio");
    MockObserver spy1, spy2;

    ufficio.addObserver(&spy1);
    ufficio.addObserver(&spy2);

    Note n("Progetto", "...");
    ufficio.addNote(&n);

    EXPECT_EQ(spy1.lastCount, 1);
    EXPECT_EQ(spy2.lastCount, 1);
    EXPECT_EQ(spy1.lastCollectionName, "Ufficio");
}

// 10. Test Robustezza: Rimozione nota non presente
TEST(CollectionTest, RemoveNonExistentNoteRobustness) {
    Collezioni collA("A");
    Collezioni collB("B");
    Note n("Nota di B", "...");

    collB.addNote(&n);

    // Tentativo di rimuovere da A una nota che appartiene a B
    // Non deve crashare e non deve cambiare il count di A
    EXPECT_NO_THROW(collA.removeNote(&n));
    EXPECT_EQ(collA.getNoteCount(), 0);
    EXPECT_EQ(collB.getNoteCount(), 1);
}

// 11. Test Persistenza Singleton
TEST(CollectionTest, SpecialCollectionPersistence) {
    Collezioni& imp = Collezioni::getImportanti();
    imp.clear(); // reset imp per test pulito, altrimenti potrebbe essere già popolato da altri test

    Note n("Key", "Value");

    imp.addNote(&n);
    {
        Collezioni temp("Temp");
        temp.addNote(&n);
        EXPECT_EQ(imp.getNoteCount(), 1);
        EXPECT_TRUE(n.isImportante());
    }

    EXPECT_TRUE(n.isImportante());
    EXPECT_EQ(imp.getNoteCount(), 1);
}