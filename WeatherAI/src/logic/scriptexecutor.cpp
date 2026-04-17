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
bool ScriptExecutor::runPythonScript(const QString &pureCode) {
    if (QFile::exists("wykres.png")) QFile::remove("wykres.png");

    // Jeśli AI dodało śmieciowe grawisy wewnątrz tagów, czyścimy je
    QString finalCode = pureCode;
    finalCode.remove("```python");
    finalCode.remove("```");

    QFile file("generated_chart.py");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream out(&file);
    out << finalCode.trimmed();
    file.close();

    QProcess py;
    py.start("python", QStringList() << "generated_chart.py");
    py.waitForFinished(30000);

    return QFile::exists("wykres.png"); // Jeśli plik powstał, to sukces
}