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
ScriptExecutor::ScriptExecutor(QObject *parent) : QObject(parent) {
    process = new QProcess(this);
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &ScriptExecutor::onProcessFinished);
    connect(process, &QProcess::errorOccurred, this, &ScriptExecutor::onProcessError);
}

/**
 * @brief Wyodrębnia, czyści i uruchamia kod Python z odpowiedzi AI asynchronicznie.
 */
void ScriptExecutor::runPythonScript(const QString &pureCode) {
    if (QFile::exists("wykres.png")) QFile::remove("wykres.png");

    // Jeśli AI dodało śmieciowe grawisy wewnątrz tagów, czyścimy je
    QString finalCode = pureCode;
    finalCode.remove("```python");
    finalCode.remove("```");

    QFile file("generated_chart.py");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit scriptFinished(false, "Nie udało się zapisać pliku skryptu.");
        return;
    }
    QTextStream out(&file);
    out << finalCode.trimmed();
    file.close();

    // Asynchroniczne uruchomienie Pythona
    process->start("python", QStringList() << "generated_chart.py");
}

void ScriptExecutor::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::NormalExit && exitCode == 0 && QFile::exists("wykres.png")) {
        emit scriptFinished(true, ""); // Sukces
    } else {
        QString errOutput = process->readAllStandardError();
        emit scriptFinished(false, "Skrypt zakończył się z błędem:\n" + errOutput);
    }
}

void ScriptExecutor::onProcessError(QProcess::ProcessError error) {
    if (error == QProcess::FailedToStart) {
        emit scriptFinished(false, "Nie znaleziono środowiska Python. Upewnij się, że Python jest zainstalowany i dodany do zmiennych PATH.");
    } else {
        emit scriptFinished(false, "Wystąpił błąd podczas uruchamiania skryptu.");
    }
}