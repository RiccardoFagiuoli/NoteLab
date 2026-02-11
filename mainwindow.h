//
// Created by Riccardo on 11/02/2026.
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <map>
#include <qlistwidget.h>
#include <string>
#include "Collezioni.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow() override;

    private Q_SLOTS:
        // Dashboard
        void on_btnDelete_clicked();
        void on_listaNote_currentRowChanged(int currentRow);
        void on_btnRendiImportante_clicked();
        void on_btnLock_clicked();
        void on_btnAddColl_clicked();
        void on_listaCollezioni_currentRowChanged(int currentRow);
        void on_btnSposta_clicked();
        void on_btnNuovaNota_clicked();    // Apre l'editor vuoto
        void on_btnModificaNota_clicked();

        //Editor
        void on_textContenutoEditor_textChanged();
        void on_btnSalvaEsciEditor_clicked();
        void on_btnAnnullaEditor_clicked();


    void on_listaNote_itemDoubleClicked(QListWidgetItem *item);

private:
        void updateUI();       // Aggiorna la lista delle note
        void updateSidebar();  // Aggiorna la lista delle collezioni con i numerini
        void updateCombo();    // Aggiorna i nomi nel menu a tendina "Sposta"

        enum Pagine {
            DASHBOARD = 0,
            EDITOR = 1
        };

        Ui::MainWindow *ui;
        Note* notaInModifica = nullptr;
        std::map<std::string, Collezioni*> dizionarioCollezioni;
        Collezioni* collezioneAttiva;
};

#endif //NOTELAB_MAINWINDOW_H