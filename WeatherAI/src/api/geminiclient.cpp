#include "geminiclient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QProcessEnvironment>
#include <QDebug>

GeminiClient::GeminiClient(QObject *parent) : QObject(parent) {
    networkManager = new QNetworkAccessManager(this);
}

/**
 * @brief Wysyła zapytanie do modelu Google Gemini.
 * Obsługuje format JSON zgodnie z dokumentacją API Gemini.
 */
void GeminiClient::sendRequest(const QString &prompt, const QString &systemContext) {
    // Odczyt klucza API ze zmiennej środowiskowej i usunięcie ewentualnych spacji/enterów na końcu
    QString apiKey = qEnvironmentVariable("GEMINI_API_KEY").trimmed();
    
    if (apiKey.isEmpty()) {
        emit errorOccurred("Brak klucza API. Ustaw zmienną środowiskową GEMINI_API_KEY.");
        return;
    }

    QUrl url("https://generativelanguage.googleapis.com/v1beta/models/gemini-flash-latest:generateContent?key=" + apiKey);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Budowanie struktury JSON dla Gemini
    QJsonObject json;
    
    // Kontekst systemowy (instrukcje)
    QJsonObject systemInstruction;
    QJsonObject systemParts;
    systemParts["text"] = systemContext;
    QJsonArray systemPartsArray;
    systemPartsArray.append(systemParts);
    systemInstruction["parts"] = systemPartsArray;
    json["system_instruction"] = systemInstruction;

    // Treść od użytkownika
    QJsonArray contents;
    QJsonObject userContent;
    userContent["role"] = "user";
    
    QJsonObject userParts;
    userParts["text"] = prompt;
    QJsonArray userPartsArray;
    userPartsArray.append(userParts);
    userContent["parts"] = userPartsArray;
    
    contents.append(userContent);
    json["contents"] = contents;

    QNetworkReply *reply = networkManager->post(request, QJsonDocument(json).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onReplyFinished(reply);
    });
}

void GeminiClient::onReplyFinished(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        QJsonObject jsonObj = jsonDoc.object();
        
        // Ekstrakcja odpowiedzi z formatu JSON Google Gemini
        if (jsonObj.contains("candidates") && jsonObj["candidates"].isArray()) {
            QJsonArray candidates = jsonObj["candidates"].toArray();
            if (!candidates.isEmpty()) {
                QJsonObject firstCandidate = candidates[0].toObject();
                if (firstCandidate.contains("content")) {
                    QJsonObject content = firstCandidate["content"].toObject();
                    if (content.contains("parts") && content["parts"].isArray()) {
                        QJsonArray parts = content["parts"].toArray();
                        if (!parts.isEmpty()) {
                            QString responseText = parts[0].toObject()["text"].toString();
                            emit responseReceived(responseText);
                            reply->deleteLater();
                            return;
                        }
                    }
                }
            }
        }
        
        emit errorOccurred("Nie udało się sparsować odpowiedzi od Gemini.");
    } else {
        // --- TA CZĘŚĆ JEST KLUCZOWA DO DEBUGOWANIA ---
        QByteArray errorData = reply->readAll();
        qDebug() << "PEŁNY BŁĄD GEMINI:" << errorData; // To pokaże nam co mówi Google!

        QString errorMsg = "Błąd API: " + reply->errorString();
        if (!errorData.isEmpty()) {
            errorMsg += " | Szczegóły: " + QString::fromUtf8(errorData);
        }
        emit errorOccurred(errorMsg);
    }
    reply->deleteLater();
}
