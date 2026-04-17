#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "scriptexecutor.h"
#include "geminiclient.h"
#include <QMessageBox>
#include <QPixmap>
#include <QDir>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
/**
 * @brief Konstruktor okna głównego.
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // Inicjalizacja klienta AI (Bielik/Ollama)
    aiClient = new OllamaClient(this);

    // Inicjalizacja klienta Gemini
    geminiClient = new GeminiClient(this);

    // Połączenie sygnałów dla Bielika
    connect(aiClient, &OllamaClient::responseReceived, this, &MainWindow::handleAIResponse);
    connect(aiClient, &OllamaClient::errorOccurred, this, &MainWindow::handleAIError);

    // Połączenie sygnałów dla Gemini
    connect(geminiClient, &GeminiClient::responseReceived, this, &MainWindow::handleAIResponse);
    connect(geminiClient, &GeminiClient::errorOccurred, this, &MainWindow::handleAIError);

    ui->statusbar->showMessage("Gotowy do pracy.");
}

MainWindow::~MainWindow() {
    delete ui;
}

/**
 * @brief Obsługa kliknięcia przycisku. Buduje dynamiczny prompt na podstawie ustawień GUI.
 */
void MainWindow::on_checkWeatherButton_clicked() {
    ui->responseText->clear();
    ui->chartDisplay->clear();
    QString city = ui->cityInput->text();

    if (city.isEmpty()) {
        QMessageBox::warning(this, "Błąd", "Proszę wpisać nazwę miasta!");
        return;
    }

    // 1. Pobieramy styl z pola tekstowego (jeśli pusty, dajemy domyślny)
    QString style = ui->styleInput->text();
    if (style.isEmpty()) style = "ekspercki, ale przystępny";

    // 2. Ustalamy o czym ma być ciekawostka (Radio Buttons)
    QString curiositySubject;
    if (ui->radioPerson->isChecked()) {
        curiositySubject = "znanej osobie pochodzącej z tego miasta";
    } else if (ui->radioRegion->isChecked()) {
        curiositySubject = "regionie lub państwie, w którym leży to miasto";
    } else {
        curiositySubject = "historii tego miasta";
    }

    // 3. Budujemy dynamiczny System Prompt - klucz do V 2.0
    QString systemPrompt = QString(
                               "Jesteś inteligentnym asystentem. TWOIM ABSOLUTNYM OBOWIĄZKIEM jest trzymać się formatu, "
                               "ale używać stylu: %1.\n\n"
                               "STRUKTURA ODPOWIEDZI (WYMAGANA):\n"
                               "1. OPIS: [Tu opis klimatu miasta %2 w wybranym stylu]\n"
                               "2. CIEKAWOSTKA: [Tu ciekawostka o %3 w wybranym stylu]\n"
                               "3. PORADA: [Tu porada o ubiorze w wybranym stylu]\n"
                               "4. Sekcja [PYTHON]: Wygeneruj kod Python (matplotlib), który stworzy DWA wykresy w jednym pionowym układzie (2 wiersze, 1 kolumna):\n"
                               "   - Górny wykres: 'Prognoza na dziś' (pogoda godzinowa, np. 8:00, 10:00, 12:00, 14:00, 16:00, 18:00, 20:00, 22:00).\n"
                               "   - Dolny wykres: 'Średnie temperatury roczne' (miesiące od Sty do Gru).\n"
                               "   - Użyj plt.subplots(2, 1, figsize=(6, 8)) i plt.tight_layout().\n"
                               "   - Zapisz jako 'wykres.png' i NIE używaj plt.show().\n\n"
                               "Cały kod Pythona musi być OTWARTY znacznikiem [PYTHON] i ZAMKNIĘTY znacznikiem [/PYTHON]. "
                               "Wewnątrz tych znaczników nie może być ŻADNEGO tekstu poza kodem.\n\n"
                               "Pamiętaj: Nawet jeśli piszesz jako %1, musisz użyć słów OPIS:, CIEKAWOSTKA:, PORADA: oraz znaczników [PYTHON] i [/PYTHON]."
                               ).arg(style, city, curiositySubject);

    // 4. Logika wyboru modelu (Bielik vs Gemini)
    if (ui->geminiCheck->isChecked()) {
        ui->statusbar->showMessage("Wysyłam zapytanie do Gemini (" + style + ") online...");
        geminiClient->sendRequest("Opowiedz mi o: " + city, systemPrompt);
        ui->checkWeatherButton->setEnabled(false); // Blokujemy przycisk na czas pracy
    } else {
        ui->statusbar->showMessage("Bielik (" + style + ") analizuje miasto: " + city + "...");
        aiClient->sendRequest("Opowiedz mi o: " + city, systemPrompt);
        ui->checkWeatherButton->setEnabled(false); // Blokujemy przycisk na czas pracy
    }
}

/**
 * @brief Przetwarza odpowiedź AI, formatuje HTML i obsługuje wykres.
 */
void MainWindow::handleAIResponse(const QString &response) {
    ui->responseText->clear();
    ui->chartDisplay->clear();
    QString userText = response;

    // 1. WYCINANIE REGEKSOWE (Pancerne)
    // Szukamy wszystkiego między [PYTHON] a [/PYTHON] (lub do końca stringa), ignorując wielkość liter i entery
    QRegularExpression pythonRegex("\\[PYTHON\\](.*?)(?:\\[/PYTHON\\]|$)",
                                   QRegularExpression::DotMatchesEverythingOption |
                                       QRegularExpression::CaseInsensitiveOption);

    QRegularExpressionMatch match = pythonRegex.match(userText);

    if (match.hasMatch()) {
        QString codeOnly = match.captured(1).trimmed(); // Sam kod bez tagów

        // Uruchamiamy skrypt (przesyłamy sam wycięty kod do ScriptExecutor)
        ui->statusbar->showMessage("Uruchamiam silnik graficzny...");
        if (ScriptExecutor::runPythonScript(codeOnly)) { // Zmienimy funkcję, żeby brała czysty kod
            QPixmap chart("wykres.png");
            if (!chart.isNull()) {
                ui->chartDisplay->setPixmap(chart.scaled(ui->chartDisplay->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
            }
        }

        // Usuwamy CAŁY dopasowany blok (razem z tagami) z tekstu dla usera
        userText.remove(match.captured(0));
    }

    // 2. CZYSZCZENIE RESZTEK (Dla pewności gdyby AI użyło grawisów ```)
    userText.remove("```python", Qt::CaseInsensitive);
    userText.remove("```");

    // 3. Nagłówki i HTML
    userText.replace("OPIS:", "<b>🌍 OPIS KLIMATU</b><br>");
    userText.replace("CIEKAWOSTKA:", "<br><b>📜 CIEKAWOSTKA HISTORYCZNA</b><br>");
    userText.replace("PORADA:", "<br><b>👔 JAK SIĘ DZISIAJ UBRAĆ?</b><br>");
    userText.replace("\n", "<br>");

    QString html = "<html><style>body{color:white;font-family:sans-serif;} b{color:#55aaff;}</style>"
                   "<body>" + userText + "</body></html>";
    ui->responseText->setHtml(html);
    ui->checkWeatherButton->setEnabled(true);
}

/**
 * @brief Obsługa błędów.
 */
void MainWindow::handleAIError(const QString &error) {
    QMessageBox::critical(this, "Błąd AI", error);
    ui->statusbar->showMessage("Wystąpił błąd podczas komunikacji.");
    ui->checkWeatherButton->setEnabled(true);
}