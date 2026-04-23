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

    // Budowanie obiektu JSON
    QJsonObject json;
    json["model"] = "SpeakLeash/bielik-11b-v3.0-instruct:Q4_K_M"; // wybieranie modelu
    json["system"] = systemContext;
    json["prompt"] = prompt;
    json["stream"] = false;

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
        if (reply->error() == QNetworkReply::ConnectionRefusedError || reply->error() == QNetworkReply::HostNotFoundError) {
            emit errorOccurred("Brak połączenia z lokalnym modelem Ollama. Upewnij się, że program Ollama jest włączony i model pobrany.");
        } else {
            emit errorOccurred("Błąd połączenia z Ollama: " + reply->errorString());
        }
    }
    reply->deleteLater();
}