/**
 * @file scriptexecutor.cpp
 * @brief Implementacja logiki odpowiedzialnej za czyszczenie i uruchamianie kodu Python.
 */

#include "scriptexecutor.h"
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QDebug>

/**
 * @brief Wyodrębnia, czyści i uruchamia kod Python z odpowiedzi AI.
 * * Funkcja usuwa stare pliki wykresów, wycina kod pomiędzy znacznikami [PYTHON],
 * usuwa ewentualne formatowanie Markdown (```python) i wykonuje skrypt.
 * * @param fullResponse Surowa odpowiedź od modelu AI.
 * @return true Jeśli skrypt zakończył się sukcesem, false w przeciwnym razie.
 */
bool ScriptExecutor::runPythonScript(const QString &fullResponse) {
    // 1. Sprzątanie przed startem
    if (QFile::exists("wykres.png")) {
        QFile::remove("wykres.png");
    }

    // 2. Szukanie znaczników logicznych
    int startTag = fullResponse.indexOf("[PYTHON]");
    int endTag = fullResponse.indexOf("[/PYTHON]");

    if (startTag == -1 || endTag == -1) {
        qDebug() << "Błąd: Nie znaleziono znaczników [PYTHON] w odpowiedzi.";
        return false;
    }

    // Wycinanie zawartości tagów
    QString code = fullResponse.mid(startTag + 8, endTag - (startTag + 8)).trimmed();

    // 3. SANITIZACJA KODU (Usuwanie ```python ... ```)
    // Jeśli AI dodało formatowanie Markdown, musimy je usunąć przed zapisem do .py
    if (code.startsWith("```")) {
        int firstNewLine = code.indexOf("\n");
        if (firstNewLine != -1) {
            code = code.mid(firstNewLine).trimmed(); // Usuwa linię "```python"
        }
    }
    if (code.endsWith("```")) {
        code.chop(3); // Usuwa końcowe "```"
        code = code.trimmed();
    }

    // 4. Zapis do pliku tymczasowego
    QFile file("generated_chart.py");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

    QTextStream out(&file);
    out << code;
    file.close();

    // 5. Uruchomienie interpretera
    QProcess pythonProcess;
    pythonProcess.start("python", QStringList() << "generated_chart.py");

    // Czekamy 30s na zakończenie (matplotlib czasem startuje powoli)
    return pythonProcess.waitForFinished(30000);
}