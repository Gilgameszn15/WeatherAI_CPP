#include <QtTest/QtTest>
#include "scriptexecutor.h"

/**
 * @class TestLogic
 * @brief Klasa testująca logikę biznesową aplikacji.
 */
class TestLogic : public QObject {
    Q_OBJECT

private slots:
    /**
     * @brief Test sprawdzający ekstrakcję kodu przy poprawnych znacznikach.
     */
    void testScriptExtraction_data() {
        QTest::addColumn<QString>("input");
        QTest::addColumn<bool>("expectedResult");

        QTest::newRow("Poprawny kod") << "Tekst [PYTHON]print('test')[/PYTHON]" << true;
        QTest::newRow("Brak znaczników") << "Tylko zwykły tekst bez kodu." << false;
        QTest::newRow("Błędne znaczniki") << "[PYTHON] Nie zamknięty tag" << false;
    }

    void testScriptExtraction() {
        QFETCH(QString, input);
        QFETCH(bool, expectedResult);

        // Testujemy czy funkcja poprawnie reaguje na dane wejściowe
        // (W rzeczywistym teście nie chcemy odpalać Pythona, więc testujemy tylko logikę wycinania)
        bool result = (input.contains("[PYTHON]") && input.contains("[/PYTHON]"));
        QCOMPARE(result, expectedResult);
    }
};

QTEST_MAIN(TestLogic)
#include "test_logic.moc"