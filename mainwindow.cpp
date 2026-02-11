//
// Created by Riccardo on 11/02/2026.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved
#include "mainwindow.h"
#include "ui_MainWindow.h"
#include "Note.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // Inizializziamo le collezioni base
    notaInModifica = nullptr;
    dizionarioCollezioni["Tutte"] = new Collezioni("Tutte");
    dizionarioCollezioni["Importanti"] = &Collezioni::getImportanti();

    collezioneAttiva = dizionarioCollezioni["Tutte"];

    updateSidebar();
    updateCombo();
    updateUI();
}

MainWindow::~MainWindow() {
    delete ui;
    // Attenzione: non cancellare getImportanti() perché è statica!
    // Cancella solo le altre collezioni create con new
}

// --- AGGIORNAMENTO INTERFACCIA ---

void MainWindow::updateSidebar() {
    ui->listaCollezioni->clear();
    for (auto const& [nome, coll] : dizionarioCollezioni) {
        QString label = QString("%1 (%2)")
                        .arg(QString::fromStdString(nome))
                        .arg(coll->getNoteCount());
        ui->listaCollezioni->addItem(label);
    }
}

void MainWindow::updateUI() {
    ui->listaNote->clear();
    ui->textAnteprima->clear();
    ui->textContenutoEditor->clear();

    for (Note* n : collezioneAttiva->getNote()) {
        ui->listaNote->addItem(QString::fromStdString(n->getTitle()));
    }
    updateSidebar(); // Aggiorna i conteggi quando cambiano le note
}

void MainWindow::updateCombo() {
    ui->comboSposta->clear();
    for (auto const& [nome, coll] : dizionarioCollezioni) {
        // AGGIUNGIAMO IL FILTRO:
        // Se il nome è "Importanti", lo saltiamo
        if (nome == "Importanti") {
            continue;
        }
        ui->comboSposta->addItem(QString::fromStdString(nome));
    }
}

// --- LOGICA NOTE ---
/*
void MainWindow::on_listaNote_currentRowChanged(int currentRow) {
    if (currentRow >= 0) {
        Note* n = collezioneAttiva->getNote()[currentRow];
        // Blocco temporaneo dei segnali per evitare loop mentre carichiamo il testo
        ui->textContenutoEditor->blockSignals(true);
        ui->textContenutoEditor->setPlainText(QString::fromStdString(n->getText()));
        ui->textContenutoEditor->blockSignals(false);
    }
} */

void MainWindow::on_listaNote_currentRowChanged(int currentRow) {
    if (currentRow >= 0 && collezioneAttiva) {
        Note* notaSelezionata = collezioneAttiva->getNote()[currentRow];

        // 1. Anteprima Testo
        ui->textAnteprima->setPlainText(QString::fromStdString(notaSelezionata->getText()));

        // 2. Aggiornamento Tasto Lock (Testo dinamico)
        ui->btnLock->setText(notaSelezionata->isLocked() ? "Sblocca" : "Blocca");

        // 3. Aggiornamento Tasto Importante (Testo dinamico)
        Collezioni* pref = dizionarioCollezioni["Importanti"];
        auto& listaNotePref = pref->getNote();

        // Cerchiamo se la nota è tra i preferiti
        auto it = std::find(listaNotePref.begin(), listaNotePref.end(), notaSelezionata);
        if (it != listaNotePref.end()) {
            ui->btnRendiImportante->setText("🌟 Rimuovi");
        } else {
            ui->btnRendiImportante->setText("⭐ Importante");
        }

        // 4. Sicurezza: Disabilita il tasto Modifica se la nota è bloccata
        ui->btnModificaNota->setEnabled(!notaSelezionata->isLocked());

        // Estetica anteprima
        if (notaSelezionata->isLocked()) {
            ui->textAnteprima->setStyleSheet("background-color: #f0f0f0; color: #888;");
        } else {
            ui->textAnteprima->setStyleSheet("");
        }
    } else {
        ui->textAnteprima->clear();
        ui->btnModificaNota->setEnabled(false);
    }
}

void MainWindow::on_textContenutoEditor_textChanged() {
    int riga = ui->listaNote->currentRow();
    if (riga >= 0 && collezioneAttiva) {
        auto& noteList = collezioneAttiva->getNote();
        if (riga < noteList.size()) { // Sicurezza extra
            Note* n = noteList[riga];
            n->setText(ui->textContenutoEditor->toPlainText().toStdString());
        }
    }
}

// --- LOGICA COLLEZIONI ---

void MainWindow::on_listaCollezioni_currentRowChanged(int currentRow) {
    if (currentRow >= 0) {
        // Estraiamo il nome pulito (senza il numero tra parentesi)
        QString testoItem = ui->listaCollezioni->currentItem()->text();
        std::string nomeColl = testoItem.split(" (")[0].toStdString();

        collezioneAttiva = dizionarioCollezioni[nomeColl];
        updateUI();
    }
}

void MainWindow::on_btnAddColl_clicked() {
    QString nome = ui->inputNuovaColl->text();
    if (!nome.isEmpty() && dizionarioCollezioni.find(nome.toStdString()) == dizionarioCollezioni.end()) {
        dizionarioCollezioni[nome.toStdString()] = new Collezioni(nome.toStdString());
        ui->inputNuovaColl->clear();
        updateSidebar();
        updateCombo();
    }
}

void MainWindow::on_btnSposta_clicked() {
    int riga = ui->listaNote->currentRow();
    if (riga < 0) return;

    Note* n = collezioneAttiva->getNote()[riga];
    QString nomeDest = ui->comboSposta->currentText();

    try {
        // Verifichiamo se la destinazione esiste nel dizionario
        if (dizionarioCollezioni.count(nomeDest.toStdString())) {
            Collezioni* destinazione = dizionarioCollezioni[nomeDest.toStdString()];

            // 1. Controllo preventivo (facoltativo ma consigliato)
            if (n->isLocked()) {
                throw std::runtime_error("Sblocca la nota prima di cambiare collezione.");
            }

            // 2. Eseguiamo lo spostamento
            // (Esempio: rimuovi dalla vecchia, aggiungi alla nuova)
            collezioneAttiva->removeNote(n);
            destinazione->addNote(n);

            updateUI();
            this->statusBar()->showMessage("Nota spostata con successo!", 3000);
        }
    }
    catch (const std::runtime_error& e) {
        // Se la nota è bloccata, il catch intercetta l'errore e mostra il popup
        QMessageBox::warning(this, "Spostamento negato", e.what());
    }
}

void MainWindow::on_btnDelete_clicked() {
    int riga = ui->listaNote->currentRow();
    if (riga < 0) return;

    Note* n = collezioneAttiva->getNote()[riga];

    try {
        // Tentiamo la rimozione
        collezioneAttiva->destructorRemove(n);

        // Se l'operazione ha successo, aggiorniamo la UI
        updateUI();
        this->statusBar()->showMessage("Nota eliminata con successo", 3000);

    } catch (const std::runtime_error& e) {
        // Se destructorRemove ha lanciato l'eccezione, finiamo qui
        QMessageBox::critical(this, "Errore di eliminazione", e.what());

        // La nota è ancora lì, non è successo nulla di grave!
    }
}
/*
void MainWindow::on_btnSalvaEsciEditor_clicked() {
    std::string titolo = ui->inputTitoloEditor->text().toStdString();
    std::string contenuto = ui->textContenutoEditor->toPlainText().toStdString();

    if (notaInModifica) {
        // Stiamo modificando una nota esistente
        if (!notaInModifica->isLocked()) {
            notaInModifica->setTitle(titolo);
            notaInModifica->setText(contenuto);
        }
    } else {
        // Stiamo creando una nota nuova
        if (!titolo.empty()) {
            Note* nuova = new Note(titolo, contenuto);
            collezioneAttiva->addNote(nuova);
            // La aggiungiamo anche alla collezione generale
            if (collezioneAttiva->getName() != "Tutte") {
                dizionarioCollezioni["Tutte"]->addNote(nuova);
            }
        }
    }
    updateUI(); // Rinfresca le liste e i conteggi
    ui->stackedWidget->setCurrentIndex(0); // Torna alla dashboard
}*/

void MainWindow::on_btnSalvaEsciEditor_clicked() {
    std::string tit = ui->inputTitoloEditor->text().toStdString();
    std::string cont = ui->textContenutoEditor->toPlainText().toStdString();

    try {
        if (notaInModifica) {
            // Se la nota è bloccata, questa riga lancerà l'eccezione "Bloccata"
            notaInModifica->setTitle(tit);
            notaInModifica->setText(cont);
        } else {
            // Creazione nuova nota
            if (!tit.empty()) {
                Note* nuova = new Note(tit, cont);
                collezioneAttiva->addNote(nuova);
                if (collezioneAttiva->getName() != "Tutte")
                    dizionarioCollezioni["Tutte"]->addNote(nuova);
            }
        }

        updateUI();
        ui->stackedWidget->setCurrentIndex(Pagine::DASHBOARD);

    } catch (const std::runtime_error& e) {
        // CATTURA IL CRASH: l'app non muore più qui
        QMessageBox::critical(this, "Errore di Salvataggio",
                             QString("Non è stato possibile salvare: %1").arg(e.what()));
    }
}

void MainWindow::on_btnLock_clicked() {
    int riga = ui->listaNote->currentRow();

    // Se siamo nell'Editor usiamo notaInModifica, altrimenti usiamo la riga selezionata
    Note* n = nullptr;
    if (ui->stackedWidget->currentIndex() == 1) { // Siamo nell'Editor
        n = notaInModifica;
    } else if (riga >= 0 && collezioneAttiva) {   // Siamo nella Dashboard
        n = collezioneAttiva->getNote()[riga];
    }

    if (n) {
        bool nuovoStato = !n->isLocked();
        n->setLocked(nuovoStato);

        // Aggiorna il testo del tasto (usa lo stesso nome oggetto che hai nel .ui)
        ui->btnLock->setText(nuovoStato ? "Sblocca" : "Blocca");

        // Se siamo nell'editor, blocca/sblocca i campi
        if (ui->stackedWidget->currentIndex() == 1) {
            ui->textContenutoEditor->setReadOnly(nuovoStato);
            ui->inputTitoloEditor->setReadOnly(nuovoStato);
        }

        // Se siamo nella dashboard, l'anteprima è sempre ReadOnly,
        // ma potresti voler cambiare il colore per feedback
        if (nuovoStato)
            ui->textAnteprima->setStyleSheet("background-color: #eeeeee;");
        else
            ui->textAnteprima->setStyleSheet("");
    }
}

void MainWindow::on_btnRendiImportante_clicked() {
    int riga = ui->listaNote->currentRow();
    if (riga >= 0) {
        Note* n = collezioneAttiva->getNote()[riga];
        Collezioni* preferiti = dizionarioCollezioni["Importanti"];

        // Controlliamo se la nota è già presente (devi avere un metodo contains nel .h)
        // Se non hai contains, puoi ciclare sulla lista di preferiti
        bool giaPresente = false;
        for(auto nota : preferiti->getNote()) {
            if(nota == n) giaPresente = true;
        }

        if (giaPresente) {
            preferiti->removeNote(n);
            ui->btnRendiImportante->setText("⭐ Rendi Importante");
        } else {
            preferiti->addNote(n);
            ui->btnRendiImportante->setText("🌟 Rimuovi Importante");
        }
        updateSidebar();
    }
}

void MainWindow::on_btnAnnullaEditor_clicked() {
    ui->stackedWidget->setCurrentIndex(Pagine::DASHBOARD); // Torna alla dashboard senza fare nulla
}

// --- TASTO CREA (PAGINA 0) ---
void MainWindow::on_btnNuovaNota_clicked() {
    // 1. Fondamentale: resetta il puntatore PRIMA di pulire l'interfaccia
    notaInModifica = nullptr;

    // 2. Disabilita temporaneamente i segnali per evitare che la pulizia
    // dei campi attivi salvataggi automatici indesiderati
    ui->inputTitoloEditor->blockSignals(true);
    ui->textContenutoEditor->blockSignals(true);

    ui->inputTitoloEditor->clear();
    ui->textContenutoEditor->clear();

    ui->inputTitoloEditor->blockSignals(false);
    ui->textContenutoEditor->setReadOnly(false);
    ui->inputTitoloEditor->setReadOnly(false);
    ui->textContenutoEditor->blockSignals(false);

    ui->stackedWidget->setCurrentIndex(1); // Vai all'editor
}

// --- TASTO MODIFICA (PAGINA 0) ---

void MainWindow::on_btnModificaNota_clicked() {
    int riga = ui->listaNote->currentRow();
    if (riga < 0) return;

    Note* n = collezioneAttiva->getNote()[riga];

    // Se per qualche motivo il tasto era cliccabile, fermiamo il crash qui
    if (n->isLocked()) {
        QMessageBox::information(this, "Nota Bloccata", "Sblocca la nota dalla dashboard per modificarla.");
        return;
    }

    // Caricamento dati nell'editor...
    notaInModifica = n;
    ui->inputTitoloEditor->setText(QString::fromStdString(n->getTitle()));
    ui->textContenutoEditor->setPlainText(QString::fromStdString(n->getText()));
    ui->stackedWidget->setCurrentIndex(Pagine::EDITOR);
}
/*
void MainWindow::on_btnModificaNota_clicked() {
    int riga = ui->listaNote->currentRow();

    if (riga >= 0 && collezioneAttiva) {
        // Prendiamo la nota selezionata dalla collezione attuale
        notaInModifica = collezioneAttiva->getNote()[riga];

        // Riempiamo l'editor con i dati della nota
        ui->inputTitoloEditor->setText(QString::fromStdString(notaInModifica->getTitle()));
        ui->textContenutoEditor->setPlainText(QString::fromStdString(notaInModifica->getText()));

        // Se la nota è bloccata, l'editor deve essere in sola lettura
        bool isLocked = notaInModifica->isLocked();
        ui->inputTitoloEditor->setReadOnly(isLocked);
        ui->textContenutoEditor->setReadOnly(isLocked);

        // Cambiamo schermata
        ui->stackedWidget->setCurrentIndex(Pagine::EDITOR);
    }
}*/

void MainWindow::on_listaNote_itemDoubleClicked(QListWidgetItem *item) {
    on_btnModificaNota_clicked();
}