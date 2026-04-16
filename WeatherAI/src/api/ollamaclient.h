/**
 * @file ollamaclient.h
 * @brief Deklaracja klasy obsługującej komunikację z API Ollama.
 */

#ifndef OLLAMACLIENT_H
#define OLLAMACLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

/**
 * @class OllamaClient
 * @brief Klasa odpowiedzialna za wysyłanie zapytań do modelu Bielik.
 * * Klasa wykorzystuje QNetworkAccessManager do asynchronicznej komunikacji
 * z lokalnym serwerem Ollama przez protokół HTTP/JSON.
 */
class OllamaClient : public QObject {
    Q_OBJECT
public:
    explicit OllamaClient(QObject *parent = nullptr);

    /**
     * @brief Wysyła zapytanie do modelu AI.
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

#endif