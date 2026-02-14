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
}


void MainWindow::updateSidebar() {
    ui->listaCollezioni->blockSignals(true);
    ui->listaCollezioni->clear();

    for (auto const& [nome, coll] : dizionarioCollezioni) {
        QString nomeQS = QString::fromStdString(nome);
        QString label = QString("%1 (%2)").arg(nomeQS).arg(coll->getNoteCount());

        QListWidgetItem* item = new QListWidgetItem(label);

        item->setData(Qt::UserRole, nomeQS);

        ui->listaCollezioni->addItem(item);
    }
    ui->listaCollezioni->blockSignals(false);
}

void MainWindow::updateUI() {

    ui->listaNote->clear();
    ui->textAnteprima->clear();

    if (!collezioneAttiva) return;

    for (Note* n : collezioneAttiva->getNote()) {
        QString suffisso = "";

        if (n->isLocked()) {
            suffisso += "🔒 ";
        }
        else{
            suffisso += "🔓 ";
        }

        if (n->isImportante()) {
            suffisso += "⭐ ";
        }

        QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(n->getTitle())+ suffisso);

        if (n->isLocked()) {
            item->setForeground(Qt::gray);
        }

        ui->listaNote->addItem(item);
    }

    updateSidebar();
}

void MainWindow::updateCombo() {
    ui->comboSposta->clear();
    for (auto const& [nome, coll] : dizionarioCollezioni) {

        if (nome == "Importanti") {
            continue;
        }
        ui->comboSposta->addItem(QString::fromStdString(nome));
    }
}


void MainWindow::on_listaNote_currentRowChanged(int currentRow) {
    if (currentRow >= 0 && collezioneAttiva) {
        Note* notaSelezionata = collezioneAttiva->getNote()[currentRow];

        ui->textAnteprima->setPlainText(QString::fromStdString(notaSelezionata->getText()));

        ui->btnLock->setText(notaSelezionata->isLocked() ? "🔓 Sblocca" : "🔒 Blocca");

        Collezioni* pref = dizionarioCollezioni["Importanti"];
        auto& listaNotePref = pref->getNote();

        auto it = std::find(listaNotePref.begin(), listaNotePref.end(), notaSelezionata);
        if (it != listaNotePref.end()) {
            ui->btnRendiImportante->setText("🌟 Rimuovi");
        } else {
            ui->btnRendiImportante->setText("⭐ Importante");
        }

        ui->btnModificaNota->setEnabled(!notaSelezionata->isLocked());

        if (notaSelezionata->isLocked()) {
            ui->textAnteprima->setStyleSheet("background-color: #424242; color: #888;");
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


void MainWindow::on_listaCollezioni_currentRowChanged(int currentRow) {
    if (currentRow >= 0) {
        QString testoItem = ui->listaCollezioni->currentItem()->text();
        std::string nomeColl = ui->listaCollezioni->currentItem()->data(Qt::UserRole).toString().toStdString();

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
    std::string nomeDest = ui->comboSposta->currentText().toStdString();

    if (collezioneAttiva->getName() == nomeDest) return;

    try {
        if (dizionarioCollezioni.count(nomeDest)) {
            Collezioni* destinazione = dizionarioCollezioni[nomeDest];

            if (n->isLocked()) throw std::runtime_error("Nota bloccata.");

            destinazione->addNote(n);

            updateUI();
            updateSidebar();
        }
    } catch (const std::runtime_error& e) {
        QMessageBox::warning(this, "Errore", e.what());
    }
}

void MainWindow::on_btnDelete_clicked() {
    int riga = ui->listaNote->currentRow();
    if (riga < 0) return;
    Note* n = collezioneAttiva->getNote()[riga];
    if (!n->isLocked()) {
        QMessageBox::StandardButton risposta;
        risposta = QMessageBox::question(this, "Conferma Eliminazione",
                                         "Sei sicuro di voler eliminare definitivamente questa nota?",
                                         QMessageBox::Yes | QMessageBox::No);
        if (risposta != QMessageBox::Yes) {
            return;
        }

        try {
            if (dizionarioCollezioni.contains("Tutte")) {
                dizionarioCollezioni["Tutte"]->destructorRemove(n);
            }
            collezioneAttiva->removeNote(n);

            delete n;

            updateUI();
            updateSidebar();
            this->statusBar()->showMessage("Nota eliminata", 3000);

        } catch (const std::runtime_error& e) {
            QMessageBox::critical(this, "Errore", e.what());
        }
    }
    else {
        QMessageBox::critical(this, "Errore", "Nota Bloccata");
    }
}

void MainWindow::on_btnSalvaEsciEditor_clicked() {
    std::string tit = ui->inputTitoloEditor->text().toStdString();
    std::string cont = ui->textContenutoEditor->toPlainText().toStdString();

    try {
        if (notaInModifica) {
            notaInModifica->setTitle(tit);
            notaInModifica->setText(cont);
        } else {
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

        QMessageBox::critical(this, "Errore di Salvataggio",
                             QString("Non è stato possibile salvare: %1").arg(e.what()));
    }
}

void MainWindow::on_btnLock_clicked() {
    int riga = ui->listaNote->currentRow();

    Note* n = nullptr;
    if (ui->stackedWidget->currentIndex() == 1) {
        n = notaInModifica;
    } else if (riga >= 0 && collezioneAttiva) {
        n = collezioneAttiva->getNote()[riga];
    }

    if (n) {
        bool nuovoStato = !n->isLocked();
        n->setLocked(nuovoStato);

        ui->btnLock->setText(nuovoStato ? "🔓 Sblocca" : "🔒 Blocca");

        if (ui->stackedWidget->currentIndex() == 1) {
            ui->textContenutoEditor->setReadOnly(nuovoStato);
            ui->inputTitoloEditor->setReadOnly(nuovoStato);
        }

        if (nuovoStato) {
            ui->textAnteprima->setStyleSheet("background-color: #424242;");
        } else {
            ui->textAnteprima->setStyleSheet("");
        }
        updateUI();
    }
}

void MainWindow::on_btnRendiImportante_clicked() {
    int riga = ui->listaNote->currentRow();
    if (riga >= 0) {
        Note* n = collezioneAttiva->getNote()[riga];
        Collezioni* preferiti = dizionarioCollezioni["Importanti"];
        /*
        bool giaPresente = false;
        for(auto nota : preferiti->getNote()) {
            if(nota == n) giaPresente = true;
        } */

        if (n->isImportante()) {
            preferiti->removeNote(n);
            ui->btnRendiImportante->setText("⭐ Rendi Importante");
        } else {
            preferiti->addNote(n);
            ui->btnRendiImportante->setText("🌟 Rimuovi Importante");
        }
        updateSidebar();
        updateUI();
    }
}

void MainWindow::on_btnAnnullaEditor_clicked() {
    ui->stackedWidget->setCurrentIndex(Pagine::DASHBOARD);
}

void MainWindow::on_btnNuovaNota_clicked() {

    notaInModifica = nullptr;

    ui->inputTitoloEditor->blockSignals(true);
    ui->textContenutoEditor->blockSignals(true);

    ui->inputTitoloEditor->clear();
    ui->textContenutoEditor->clear();

    ui->inputTitoloEditor->blockSignals(false);
    ui->textContenutoEditor->setReadOnly(false);
    ui->inputTitoloEditor->setReadOnly(false);
    ui->textContenutoEditor->blockSignals(false);

    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_btnModificaNota_clicked() {
    int riga = ui->listaNote->currentRow();
    if (riga < 0) return;

    Note* n = collezioneAttiva->getNote()[riga];

    if (n->isLocked()) {
        QMessageBox::information(this, "Nota Bloccata", "Sblocca la nota dalla dashboard per modificarla.");
        return;
    }


    notaInModifica = n;
    ui->inputTitoloEditor->setText(QString::fromStdString(n->getTitle()));
    ui->textContenutoEditor->setPlainText(QString::fromStdString(n->getText()));
    ui->stackedWidget->setCurrentIndex(Pagine::EDITOR);
}

void MainWindow::on_listaNote_itemDoubleClicked(QListWidgetItem *item) {
    on_btnModificaNota_clicked();
}

void MainWindow::on_btnEliminaCollezione_clicked() {
    std::string nomeDaEliminare = collezioneAttiva->getName();

    if (nomeDaEliminare == "Tutte" || nomeDaEliminare == "Importanti") {
        QMessageBox::warning(this, "Attenzione", "Non puoi eliminare questa collezione.");
        return;
    }

    if (QMessageBox::question(this, "Conferma", "Vuoi eliminare la collezione? Le note rimarranno in 'Tutte'.") == QMessageBox::Yes) {

        Collezioni* daCancellare = dizionarioCollezioni[nomeDaEliminare];
        dizionarioCollezioni.erase(nomeDaEliminare);
        delete daCancellare;

        collezioneAttiva = dizionarioCollezioni["Tutte"];

        updateSidebar();
        updateCombo();

        for(int i = 0; i < ui->listaCollezioni->count(); ++i) {
            QListWidgetItem* item = ui->listaCollezioni->item(i);
            if (item->data(Qt::UserRole).toString() == "Tutte") {
                ui->listaCollezioni->setCurrentItem(item);
                break;
            }
        }

        updateUI();

        this->statusBar()->showMessage("Collezione eliminata con successo.", 3000);
    }
}