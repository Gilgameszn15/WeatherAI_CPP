/**
 * @file mainwindow.h
 * @brief Główne okno aplikacji Pogodynka AI.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ollamaclient.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @class MainWindow
 * @brief Klasa zarządzająca interfejsem użytkownika.
 * * Obsługuje interakcje z użytkownikiem, wyświetlanie danych
 * oraz prezentację wygenerowanych wykresów.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    /**
     * @brief Slot wywoływany po kliknięciu przycisku sprawdzania pogody.
     */
    void on_checkWeatherButton_clicked();

    /**
     * @brief Obsługuje i wyświetla odpowiedź tekstową od AI.
     * @param response Tekst odpowiedzi.
     */
    void handleAIResponse(const QString &response);

    /**
     * @brief Wyświetla błąd w oknie dialogowym.
     * @param error Treść błędu.
     */
    void handleAIError(const QString &error);

private:
    Ui::MainWindow *ui; ///< Wskaźnik na elementy interfejsu użytkownika.
    OllamaClient *aiClient; ///< Instancja klienta komunikującego się z AI.
};

#endif