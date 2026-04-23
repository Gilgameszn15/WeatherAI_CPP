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
        QTest::newRow("Brak znacznikow") << "Tylko zwykly tekst bez kodu." << false;
        QTest::newRow("Brak zamkniecia (nowy regex to lapie)") << "Jakis tekst [PYTHON] niezamkniety kod..." << true;
    }

    void testScriptExtraction() {
        QFETCH(QString, input);
        QFETCH(bool, expectedResult);

        // Używamy tego samego wyrażenia regularnego, co w MainWindow
        QRegularExpression pythonRegex("\\[PYTHON\\](.*?)(?:\\[/PYTHON\\]|$)",
                                       QRegularExpression::DotMatchesEverythingOption |
                                       QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch match = pythonRegex.match(input);
        bool result = match.hasMatch();
        QCOMPARE(result, expectedResult);
    }
};

QTEST_MAIN(TestLogic)
#include "test_logic.moc"