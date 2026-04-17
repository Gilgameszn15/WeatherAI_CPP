#include "ollamaclient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QDebug>

OllamaClient::OllamaClient(QObject *parent) : QObject(parent) {
    networkManager = new QNetworkAccessManager(this);
}

/**
 * @brief Wysyła zapytanie do modelu Bielik.
 * Obsługuje format JSON zgodnie z dokumentacją Ollama [cite: 19, 20-25].
 */
void OllamaClient::sendRequest(const QString &prompt, const QString &systemContext) {
    QUrl url("http://localhost:11434/api/generate");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Budowanie obiektu JSON [cite: 22-25]
    QJsonObject json;
    json["model"] = "SpeakLeash/bielik-11b-v3.0-instruct:Q4_K_M"; // Możesz zmienić na "llama3" jeśli wolisz
    json["system"] = systemContext; // "system prompt" [cite: 10]
    json["prompt"] = prompt;        // "user prompt" [cite: 10]
    json["stream"] = false;         // Wyłączamy strumieniowanie dla prostoty [cite: 25]

    QNetworkReply *reply = networkManager->post(request, QJsonDocument(json).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onReplyFinished(reply);
    });
}

void OllamaClient::onReplyFinished(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        QString responseText = jsonDoc.object().value("response").toString();
        emit responseReceived(responseText);
    } else {
        // Obsługa braku łączności (wymóg na 5.0) [cite: 27, 28, 59]
        if (reply->error() == QNetworkReply::ConnectionRefusedError || reply->error() == QNetworkReply::HostNotFoundError) {
            emit errorOccurred("Brak połączenia z lokalnym modelem Ollama. Upewnij się, że program Ollama jest włączony i model pobrany.");
        } else {
            emit errorOccurred("Błąd połączenia z Ollama: " + reply->errorString());
        }
    }
    reply->deleteLater();
}