/**
 * @file scriptexecutor.h
 * @brief Deklaracja klasy odpowiedzialnej za automatyzację skryptów Python.
 */

#ifndef SCRIPTEXECUTOR_H
#define SCRIPTEXECUTOR_H

#include <QObject>
#include <QString>
#include <QProcess>

/**
 * @class ScriptExecutor
 * @brief Klasa narzędziowa do ekstrakcji i uruchamiania kodu Python.
 */
class ScriptExecutor : public QObject {
    Q_OBJECT
public:
    explicit ScriptExecutor(QObject *parent = nullptr);

    /**
     * @brief Wyodrębnia kod Python z odpowiedzi AI i uruchamia go asynchronicznie.
     * @param pureCode Czysty kod od modelu AI.
     */
    void runPythonScript(const QString &pureCode);

signals:
    /**
     * @brief Sygnał emitowany po zakończeniu działania skryptu (sukces lub błąd).
     * @param success Czy skrypt wygenerował wykres poprawnie.
     * @param errorMessage Opcjonalny komunikat błędu (np. brak Pythona).
     */
    void scriptFinished(bool success, const QString &errorMessage);

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);

private:
    QProcess *process;
};

#endif // SCRIPTEXECUTOR_H