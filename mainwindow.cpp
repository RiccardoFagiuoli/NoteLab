//
// Created by Riccardo on 11/02/2026.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved
#include "mainwindow.h"
#include "ui_MainWindow.h"
#include "Note.h"

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
    ui->textContenutoEditor->clear();

    for (Note* n : collezioneAttiva->getNote()) {
        ui->listaNote->addItem(QString::fromStdString(n->getTitle()));
    }
    updateSidebar(); // Aggiorna i conteggi quando cambiano le note
}

void MainWindow::updateCombo() {
    ui->comboSposta->clear();
    for (auto const& [nome, coll] : dizionarioCollezioni) {
        ui->comboSposta->addItem(QString::fromStdString(nome));
    }
}

// --- LOGICA NOTE ---

void MainWindow::on_listaNote_currentRowChanged(int currentRow) {
    if (currentRow >= 0) {
        Note* n = collezioneAttiva->getNote()[currentRow];
        // Blocco temporaneo dei segnali per evitare loop mentre carichiamo il testo
        ui->textContenutoEditor->blockSignals(true);
        ui->textContenutoEditor->setPlainText(QString::fromStdString(n->getText()));
        ui->textContenutoEditor->blockSignals(false);
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

void MainWindow::on_btnAdd_clicked() {
    QString titolo = ui->inputTitoloEditor->text();
    if (!titolo.isEmpty()) {
        Note* nuova = new Note(titolo.toStdString(), "");
        collezioneAttiva->addNote(nuova);

        // Se non siamo già in "Tutte", aggiungiamola anche lì
        if (collezioneAttiva != dizionarioCollezioni["Tutte"])
            dizionarioCollezioni["Tutte"]->addNote(nuova);

        ui->inputTitoloEditor->clear();
        updateUI();
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
    int rigaNota = ui->listaNote->currentRow();
    std::string nomeDest = ui->comboSposta->currentText().toStdString();

    if (rigaNota >= 0 && collezioneAttiva->getName() != nomeDest) {
        Note* n = collezioneAttiva->getNote()[rigaNota];
        dizionarioCollezioni[nomeDest]->addNote(n);
        collezioneAttiva->removeNote(n);
        updateUI();
    }
}

void MainWindow::on_btnDelete_clicked() {
    int riga = ui->listaNote->currentRow();
    if (riga >= 0 && collezioneAttiva) {
        // Recuperiamo la nota
        Note* notaDaEliminare = collezioneAttiva->getNote()[riga];

        // Se la nota è in più collezioni, il tuo distruttore dovrebbe gestirlo.
        // Altrimenti, per ora, la eliminiamo fisicamente:
        delete notaDaEliminare;

        updateUI(); // Rinfresca la lista e i conteggi
    }
}

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
}

void MainWindow::on_btnLock_clicked() {
    if (notaInModifica) {
        // Cambia lo stato della nota
        bool nuovoStato = !notaInModifica->isLocked();
        notaInModifica->setLocked(nuovoStato);

        // Blocca/Sblocca l'interfaccia dell'editor
        ui->textContenutoEditor->setReadOnly(nuovoStato);
        ui->inputTitoloEditor->setReadOnly(nuovoStato);

        // Cambia l'estetica del tasto per feedback visivo
        ui->btnLock->setText(nuovoStato ? "🔓 Sblocca" : "🔒 Blocca");
    }
}

void MainWindow::on_btnRendiImportante_clicked() {
    if (notaInModifica) {
        // Aggiunge la nota alla collezione speciale senza rimuoverla dalla corrente
        dizionarioCollezioni["Importanti"]->addNote(notaInModifica);
        // Suggerimento: potresti cambiare il colore del tasto per dire che è fatto
        ui->btnRendiImportante->setStyleSheet("background-color: yellow;");
    }
}

void MainWindow::on_btnAnnullaEditor_clicked() {
    ui->stackedWidget->setCurrentIndex(Pagine::DASHBOARD); // Torna alla dashboard senza fare nulla
}

// --- TASTO CREA (PAGINA 0) ---
void MainWindow::on_btnNuovaNota_clicked() {
    notaInModifica = nullptr; // Diciamo all'editor: "Stiamo creando da zero"

    // Puliamo i campi dell'editor (Pagina 1) prima di andarci
    ui->inputTitoloEditor->clear();
    ui->textContenutoEditor->clear();

    // Riabilitiamo i campi nel caso fossero stati bloccati da una nota precedente
    ui->inputTitoloEditor->setReadOnly(false);
    ui->textContenutoEditor->setReadOnly(false);

    // Cambiamo schermata: 1 è l'indice della pagina dell'Editor
    ui->stackedWidget->setCurrentIndex(Pagine::EDITOR);
}

// --- TASTO MODIFICA (PAGINA 0) ---
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
}

void MainWindow::on_listaNote_itemDoubleClicked(QListWidgetItem *item) {
    on_btnModificaNota_clicked();
}
