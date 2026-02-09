//
// Created by Riccardo on 09/02/2026.
//

#include <gtest/gtest.h>
#include <stdexcept>
#include "Note.h"
#include "Collezioni.h"
#include "Observer.h"

// Questa classe gestisce la preparazione e la pulizia per ogni test
class CollectionTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        // Viene eseguito PRIMA di ogni test
        // Svuota la collezione Singleton per evitare residui dai test precedenti
        Collezioni::getImportanti().clear();
    }

    void TearDown() override {
        // Viene eseguito DOPO ogni test
        // Utile se avessimo allocazioni dinamiche globali da pulire
    }
};

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

// --- TEST---

// 1. Verifica proprietà base della Nota e meccanismo di Lock
TEST_F(CollectionTestFixture, NoteLockAndProperties) {
    Note n("Titolo", "Contenuto");
    n.setLocked(true);

    EXPECT_THROW(n.setTitle("Nuovo"), std::runtime_error);

    n.setLocked(false);
    EXPECT_NO_THROW(n.setTitle("Nuovo Titolo"));
    EXPECT_EQ(n.getTitle(), "Nuovo Titolo");
}

// 2. Verifica esclusività tra Collezioni NORMALI
TEST_F(CollectionTestFixture, NormalCollectionExclusivity) {
    Collezioni lavoro("Lavoro");
    Collezioni casa("Casa");
    Note n("Nota Test", "...");

    lavoro.addNote(&n);
    EXPECT_EQ(lavoro.getNoteCount(), 1);
    EXPECT_EQ(n.getCollezione(), &lavoro);

    // Spostamento automatico
    casa.addNote(&n);
    EXPECT_EQ(lavoro.getNoteCount(), 0);
    EXPECT_EQ(casa.getNoteCount(), 1);
    EXPECT_EQ(n.getCollezione(), &casa);
}

// 3. Verifica Doppia Appartenenza (Normale + Importanti)
TEST_F(CollectionTestFixture, DualAppartenance) {
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

// 4. Verifica Pattern Observer (Notifica singola)
TEST_F(CollectionTestFixture, ObserverNotification) {
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

// 5. Verifica Notifica a Osservatori Multipli (Broadcast)
TEST_F(CollectionTestFixture, MultipleObserversBroadcast) {
    Collezioni ufficio("Ufficio");
    MockObserver spy1, spy2;

    ufficio.addObserver(&spy1);
    ufficio.addObserver(&spy2);

    Note n("Progetto", "...");
    ufficio.addNote(&n);

    EXPECT_EQ(spy1.lastCount, 1);
    EXPECT_EQ(spy2.lastCount, 1);
}

// 6. Verifica protezione rimozione nota bloccata
TEST_F(CollectionTestFixture, DenyRemovalIfLocked) {
    Collezioni coll("Sicura");
    Note n("Privato", "...");
    coll.addNote(&n);
    n.setLocked(true);

    EXPECT_THROW(coll.removeNote(&n), std::runtime_error);
    EXPECT_EQ(coll.getNoteCount(), 1);
}

// 7. Verifica persistenza collezione normale dopo rimozione da Importanti
TEST_F(CollectionTestFixture, LeaveImportantStayNormal) {
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

// 8. Gestione Nota Orfana (Puntatori nulli)
TEST_F(CollectionTestFixture, OrphanNoteManagement) {
    Collezioni studio("Studio");
    Note n("Appunti", "...");

    EXPECT_EQ(n.getCollezione(), nullptr);

    studio.addNote(&n);
    EXPECT_EQ(n.getCollezione(), &studio);

    studio.removeNote(&n);
    EXPECT_EQ(n.getCollezione(), nullptr);
}

// 9. Robustezza: Rimozione nota non appartenente
TEST_F(CollectionTestFixture, RemoveNonExistentNote) {
    Collezioni collA("A");
    Collezioni collB("B");
    Note n("Nota di B", "...");

    collB.addNote(&n);

    // Non deve fare nulla e non deve crashare
    EXPECT_NO_THROW(collA.removeNote(&n));
    EXPECT_EQ(collA.getNoteCount(), 0);
    EXPECT_EQ(collB.getNoteCount(), 1);
}

// 10. Persistenza Singleton (grazie a SetUp partiamo da count 0)
TEST_F(CollectionTestFixture, SpecialCollectionPersistence) {
    Collezioni& imp = Collezioni::getImportanti();
    Note n("Key", "Value");

    imp.addNote(&n);
    {
        Collezioni temp("Temp");
        temp.addNote(&n);
        EXPECT_EQ(imp.getNoteCount(), 1);
    }

    // n rimane in importanti anche se 'temp' è distrutta
    EXPECT_TRUE(n.isImportante());
    EXPECT_EQ(imp.getNoteCount(), 1);
}