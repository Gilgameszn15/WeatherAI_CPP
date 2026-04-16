#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "scriptexecutor.h"
#include <QMessageBox>
#include <QPixmap>
#include <QDir>

/**
 * @brief Konstruktor okna głównego.
 * Inicjalizuje klienta AI i łączy sygnały asynchroniczne.
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // Inicjalizacja klienta AI
    aiClient = new OllamaClient(this);

    // Połączenie sygnałów z AI z funkcjami obsługi w GUI (Wielowątkowość)
    connect(aiClient, &OllamaClient::responseReceived, this, &MainWindow::handleAIResponse);
    connect(aiClient, &OllamaClient::errorOccurred, this, &MainWindow::handleAIError);

    ui->statusbar->showMessage("Gotowy do pracy.");
}

MainWindow::~MainWindow() {
    delete ui;
}

/**
 * @brief Obsługa kliknięcia przycisku. Pobiera miasto i wysyła zapytanie.
 */
void MainWindow::on_checkWeatherButton_clicked() {
    // 1. Pobieramy nazwę miasta z GUI (zamiast sztywnego wpisu)
    QString city = ui->cityInput->text();

    if (city.isEmpty()) {
        QMessageBox::warning(this, "Błąd", "Proszę wpisać nazwę miasta!");
        return;
    }

    // 2. Definicja ról i instrukcji (System Prompt) - klucz do 5.0
    QString systemPrompt = "Jesteś ekspertem meteo. TWOJA ODPOWIEDŹ MUSI ZAWIERAĆ TRZY SEKCJE:\n"
                           "1. Opis klimatu.\n"
                           "2. Ciekawostkę historyczną.\n"
                           "3. Kod Python w znacznikach [PYTHON] i [/PYTHON].\n"
                           "INSTRUKCJE DLA KODU:\n"
                           "- Biblioteka: matplotlib.pyplot.\n"
                           "- Wykres LINIOWY średnich temperatur.\n"
                           "- Zapis pliku: plt.savefig('wykres.png').\n"
                           "- Zapisz plik DOKŁADNIE jako 'wykres.png'. NIE używaj nazw miast w nazwie pliku. To krytyczne dla działania aplikacji.\n"
                           "- BRAK komendy plt.show().\n"
                           "NIGDY NIE POMIJAJ KODU PYTHON.";

    // 3. Wysyłanie zapytania
    aiClient->sendRequest("Opowiedz mi o pogodzie i ciekawostkach w: " + city, systemPrompt);

    ui->statusbar->showMessage("Bielik myśli nad miastem: " + city + "...");
    ui->checkWeatherButton->setEnabled(false); // Blokujemy przycisk na czas pracy
}

/**
 * @brief Przetwarza odpowiedź otrzymaną od AI i aktualizuje interfejs.
 * * Czyści tekst z tagów technicznych, formatuje go przy użyciu HTML/CSS
 * i wywołuje proces generowania wykresu.
 * * @param response Surowy tekst z modelu Bielik.
 */
void MainWindow::handleAIResponse(const QString &response) {
    QString userText = response;

    // 1. Usuwanie znaczników [PYTHON] i wszystkiego pomiędzy nimi z tekstu dla użytkownika
    int pythonStart = userText.indexOf("[PYTHON]");
    int pythonEnd = userText.indexOf("[/PYTHON]");
    if (pythonStart != -1 && pythonEnd != -1) {
        userText.remove(pythonStart, (pythonEnd + 9) - pythonStart);
    }

    // 2. Czyszczenie "śmieciowych" nagłówków Markdowna
    userText.replace("### [PYTHON]", "");
    userText.replace("### Kod Python", "");
    userText.replace("### Opis klimatu", "<b>Opis klimatu</b>");
    userText.replace("### Ciekawostka historyczna", "<b>Ciekawostka historyczna</b>");

    // Usuwamy zbędne znaki # na końcach lub w tekście
    userText.replace("###", "");
    userText = userText.trimmed();

    // 3. Formatowanie HTML dla czytelności (Biały tekst, niebieskie nagłówki)
    userText.replace("\n", "<br>");
    QString html = "<html><style>"
                   "body { color: white; font-family: sans-serif; line-height: 1.4; }"
                   "b { color: #55aaff; font-size: 14px; }"
                   "</style><body>" + userText + "</body></html>";

    ui->responseText->setHtml(html);

    // 4. Wykonanie logiki wykresu
    ui->statusbar->showMessage("Generowanie grafiki...");
    // Wymuszamy odświeżenie UI przed ciężką operacją
    qApp->processEvents();

    if (ScriptExecutor::runPythonScript(response)) {
        QPixmap chart("wykres.png");
        if (!chart.isNull()) {
            ui->chartDisplay->setPixmap(chart.scaled(ui->chartDisplay->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
            ui->statusbar->showMessage("Dane zaktualizowane poprawnie.");
        } else {
            ui->statusbar->showMessage("Błąd: Nie utworzono pliku graficznego.");
        }
    } else {
        ui->statusbar->showMessage("Błąd: Skrypt AI zawiera błędy.");
    }

    ui->checkWeatherButton->setEnabled(true);
}

/**
 * @brief Obsługa błędów sieciowych lub błędów modelu.
 */
void MainWindow::handleAIError(const QString &error) {
    QMessageBox::critical(this, "Błąd komunikacji AI", error);
    ui->statusbar->showMessage("Wystąpił błąd.");
    ui->checkWeatherButton->setEnabled(true);
}