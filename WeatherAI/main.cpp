/**
 * @mainpage WeatherAI - Dokumentacja Projektu
 *
 * @section intro_sec Wprowadzenie
 * Aplikacja **WeatherAI** (Pogodynka AI) to projekt zaliczeniowy zrealizowany w języku C++ z wykorzystaniem biblioteki Qt6.
 * Program komunikuje się z dużymi modelami językowymi (LLM), takimi jak lokalny model **Bielik** (przez Ollama) 
 * oraz chmurowy model **Gemini 2.5 Flash** (Google API).
 *
 * @section features_sec Funkcjonalności
 * - 🌍 **Opis pogody i klimatu**: Generowanie szczegółowych informacji meteorologicznych.
 * - 📜 **Ciekawostki historyczne**: Dynamiczne zapytania bazujące na parametrach z GUI.
 * - 📊 **Asynchroniczne wykresy**: Użycie `ScriptExecutor` do automatycznego wycinania kodu Pythona (tagi `[PYTHON]`),
 *   uruchamiania go w tle bez blokowania GUI i renderowania wyników (wykres `matplotlib`) bezpośrednio w interfejsie.
 *
 * @section run_sec Uruchomienie
 * Instrukcje dotyczące kompilacji i uruchomienia (w tym instalacji środowiska Python) znajdują się w pliku `README.txt`.
 *
 * @author Patryk Nalewalski
 */

#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return QCoreApplication::exec();
}
