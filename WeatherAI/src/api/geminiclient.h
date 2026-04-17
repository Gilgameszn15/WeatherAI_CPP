/**
 * @file geminiclient.h
 * @brief Deklaracja klasy obsługującej komunikację z API Google Gemini.
 */

#ifndef GEMINICLIENT_H
#define GEMINICLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

/**
 * @class GeminiClient
 * @brief Klasa odpowiedzialna za wysyłanie zapytań do modelu Google Gemini.
 *
 * Klasa wykorzystuje QNetworkAccessManager do asynchronicznej komunikacji
 * z chmurowym API Google Gemini przez protokół HTTP/JSON.
 */
class GeminiClient : public QObject {
    Q_OBJECT
public:
    explicit GeminiClient(QObject *parent = nullptr);

    /**
     * @brief Wysyła zapytanie do modelu AI Gemini.
     * @param prompt Treść zapytania od użytkownika.
     * @param systemContext Kontekst systemowy (instrukcje dla AI).
     */
    void sendRequest(const QString &prompt, const QString &systemContext);

signals:
    /**
     * @brief Emitowany, gdy model AI zwróci poprawną odpowiedź.
     * @param response Pełna treść odpowiedzi od modelu.
     */
    void responseReceived(const QString &response);

    /**
     * @brief Emitowany w przypadku wystąpienia błędu sieci lub API.
     * @param errorMsg Treść komunikatu o błędzie.
     */
    void errorOccurred(const QString &errorMsg);

private slots:
    /**
     * @brief Slot obsługujący zakończenie zapytania sieciowego.
     * @param reply Wskaźnik na odpowiedź sieciową.
     */
    void onReplyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *networkManager; ///< Zarządca połączeń sieciowych.
};

#endif // GEMINICLIENT_H
