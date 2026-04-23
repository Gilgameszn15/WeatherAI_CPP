/**
 * @file scriptexecutor.cpp
 * @brief Implementacja logiki odpowiedzialnej za czyszczenie i uruchamianie kodu Python.
 */

#include "scriptexecutor.h"
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <thread>
#include <stdexcept>

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

    // Utworzenie nowego wątku
    std::thread workerThread([this, finalCode]() {
        try {
            QFile file("generated_chart.py");
            if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                // Rzucenie wyjątku
                throw std::runtime_error("Nie udało się otworzyć pliku wygenerowanego skryptu do zapisu.");
            }
            QTextStream out(&file);
            out << finalCode.trimmed();
            file.close();

            // Bezpieczne oddelegowanie uruchomienia QProcess z powrotem do głównego wątku
            QMetaObject::invokeMethod(this, [this]() {
                process->start("python", QStringList() << "generated_chart.py");
            }, Qt::QueuedConnection);

        } catch (const std::exception& e) {
            // Złapanie wyjątku i poinformowanie GUI
            QMetaObject::invokeMethod(this, [this, e]() {
                emit scriptFinished(false, QString("Wyjątek podczas pracy wątku: ") + e.what());
            }, Qt::QueuedConnection);
        }
    });

    // Odłączenie wątku
    workerThread.detach();
}

void ScriptExecutor::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::NormalExit && exitCode == 0 && QFile::exists("wykres.png")) {
        emit scriptFinished(true, "");
    } else {
        QString errOutput = process->readAllStandardError();
        emit scriptFinished(false, "Skrypt zakonczył sie z bledem:\n" + errOutput);
    }
}

void ScriptExecutor::onProcessError(QProcess::ProcessError error) {
    if (error == QProcess::FailedToStart) {
        emit scriptFinished(false, "Nie znaleziono srodowiska Python. Upewnij sie, ze Python jest zainstalowany i dodany do zmiennych PATH.");
    } else {
        emit scriptFinished(false, "Wystapil blad podczas uruchamiania skryptu.");
    }
}