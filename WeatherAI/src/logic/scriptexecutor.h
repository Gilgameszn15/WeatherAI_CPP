/**
 * @file scriptexecutor.h
 * @brief Deklaracja klasy odpowiedzialnej za automatyzację skryptów Python.
 */

#ifndef SCRIPTEXECUTOR_H
#define SCRIPTEXECUTOR_H

#include <QString>

/**
 * @class ScriptExecutor
 * @brief Klasa narzędziowa do ekstrakcji i uruchamiania kodu Python.
 */
class ScriptExecutor {
public:
    /**
     * @brief Wyodrębnia kod Python z odpowiedzi AI i uruchamia go.
     * @param fullResponse Tekst od modelu Bielik.
     * @return true Jeśli skrypt został wykonany pomyślnie.
     */
    static bool runPythonScript(const QString &fullResponse);
};

#endif // SCRIPTEXECUTOR_H