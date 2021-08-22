#include "lycheeUtils.h"
#include <QtNetwork>
#include <QJsonObject>
#include <QMessageBox>
#include <QtDebug>

QString LycheeUtils::url = "";
QString LycheeUtils::username = "";
QString LycheeUtils::password = "";

LycheeUtils::LycheeUtils(QObject *parent)
    : QObject(parent)
{

}

LycheeUtils::~LycheeUtils()
{

}

bool LycheeUtils::upload(QString path)
{
    if(path=="")
    {
        emit finished(this, "");
    }
    this->path = path;
    init();

    return false;
}

void LycheeUtils::setCommonHeader(QNetworkRequest &req)
{
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
}

void LycheeUtils::init()
{
    QUrl initUrl(LycheeUtils::url + "/api/Session::init");
    QNetworkRequest request(initUrl);
    QByteArray data;

    reply = qnam.post(request, data);
    connect(reply, &QNetworkReply::finished, this, &LycheeUtils::initFinished);
    connect(reply, &QNetworkReply::sslErrors, this, &LycheeUtils::sslErrors);
}

void LycheeUtils::initFinished()
{
    QByteArray data = reply->readAll();
    QString s(data);
    debugPrint(s);

    if(setError()) return;

    reply->deleteLater();
    reply = Q_NULLPTR;

    login();
}

void LycheeUtils::login()
{
    QUrl loginUrl(LycheeUtils::url + "/api/Session::login");
    QNetworkRequest request(loginUrl);
    QJsonObject body;
    QNetworkCookie XSRF;

    foreach(QNetworkCookie cookie, qnam.cookieJar()->cookiesForUrl(url))
    {
        if(cookie.name()=="XSRF-TOKEN")
        {
            XSRF = cookie;
            break;
        }
    }
    request.setRawHeader("X-" + XSRF.name(),XSRF.value().mid(0,XSRF.value().indexOf('%')));
    request.setHeader(QNetworkRequest::LocationHeader, QUrl(url));

    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    body.insert("function","Session::login");
    body.insert("username",LycheeUtils::username);
    body.insert("password",LycheeUtils::password);

    reply = qnam.post(request,QJsonDocument(body).toJson(QJsonDocument::Compact));
    connect(reply, &QNetworkReply::finished, this, &LycheeUtils::loginFinished);
    connect(reply, &QNetworkReply::sslErrors, this, &LycheeUtils::sslErrors);
}

void LycheeUtils::loginFinished()
{
    QByteArray data = reply->readAll();
    QString s(data);
    debugPrint(s);

    if(setError()) return;

    if(s=="true")
    {
        upload();
    }
    else if(s=="false")
    {

    }
    else
    {

    }
}

void LycheeUtils::upload()
{
    QUrl loginUrl(LycheeUtils::url + "/api/Photo::add");
    QNetworkRequest request(loginUrl);
    QHttpMultiPart *data = new QHttpMultiPart();
    QNetworkCookie XSRF;

    foreach(QNetworkCookie cookie, qnam.cookieJar()->cookiesForUrl(url))
    {
        if(cookie.name()=="XSRF-TOKEN")
        {
            XSRF = cookie;
            break;
        }
    }
    request.setRawHeader("X-" + XSRF.name(),XSRF.value().mid(0,XSRF.value().indexOf('%')));
    request.setHeader(QNetworkRequest::LocationHeader, QUrl(LycheeUtils::url));

    QFile *file = new QFile(path);
    file->open(QIODevice::ReadOnly);

    QHttpPart funcPart;
    funcPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"function\""));
    funcPart.setBody("Photo::add");
    QHttpPart textPart;
    textPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"albumID\""));
    textPart.setBody("0");
    QHttpPart imagePart;
    // imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/" + QFileInfo(path).suffix()));
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"0\"; filename=\"" + QFileInfo(path).fileName() + "\""));
    imagePart.setBodyDevice(file);
    file->setParent(data); // we cannot delete the file now, so delete it with the data

    data->setContentType(QHttpMultiPart::FormDataType);
    data->append(textPart);
    data->append(imagePart);

    reply = qnam.post(request, data);
    connect(reply, &QNetworkReply::finished, this, &LycheeUtils::uploadFinished);
    connect(reply, &QNetworkReply::sslErrors, this, &LycheeUtils::sslErrors);
    data->setParent(reply); // delete the multiPart with the reply
}

void LycheeUtils::uploadFinished()
{
    QByteArray data = reply->readAll();
    QString s(data);
    debugPrint(s);

    if(setError()) return;

    reply->deleteLater();
    reply = Q_NULLPTR;

    getUrl(s);
}

void LycheeUtils::getUrl(QString id)
{
    QUrl getUrlUrl(LycheeUtils::url + "/api/Photo::get");
    QNetworkRequest request(getUrlUrl);
    QJsonObject body;
    QNetworkCookie XSRF;

    foreach(QNetworkCookie cookie, qnam.cookieJar()->cookiesForUrl(url))
    {
        if(cookie.name()=="XSRF-TOKEN")
        {
            XSRF = cookie;
            break;
        }
    }
    request.setRawHeader("X-" + XSRF.name(),XSRF.value().mid(0,XSRF.value().indexOf('%')));
    request.setHeader(QNetworkRequest::LocationHeader, QUrl(LycheeUtils::url));

    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    body.insert("function","Photo::get");
    body.insert("photoID",id);
    body.insert("password","");

    reply = qnam.post(request,QJsonDocument(body).toJson(QJsonDocument::Compact));
    connect(reply, &QNetworkReply::finished, this, &LycheeUtils::getUrlFinished);
    connect(reply, &QNetworkReply::sslErrors, this, &LycheeUtils::sslErrors);
}

void LycheeUtils::getUrlFinished()
{
    QByteArray data = reply->readAll();
    QString s(data);
    debugPrint(s);

    if(setError()) return;

    if(reply->request().rawHeader("Content-Type") == "application/json")
    {
        QJsonDocument json = QJsonDocument::fromJson(data);
        QUrl resolvedUrl = QUrl(LycheeUtils::url).resolved(json.object().value("url").toString());
        QString picUrl = resolvedUrl.url();

        emit finished(this, picUrl);
    }
    else
    {
        setError(true);
        return;
    }

    reply->deleteLater();
    reply = Q_NULLPTR;

    logout();
}

void LycheeUtils::logout()
{
    QUrl initUrl(LycheeUtils::url + "/api/Session::logout");
    QNetworkRequest request(initUrl);
    QByteArray data;
    QNetworkCookie XSRF;

    foreach(QNetworkCookie cookie, qnam.cookieJar()->cookiesForUrl(url))
    {
        if(cookie.name()=="XSRF-TOKEN")
        {
            XSRF = cookie;
            break;
        }
    }
    request.setRawHeader("X-" + XSRF.name(),XSRF.value().mid(0,XSRF.value().indexOf('%')));

    reply = qnam.post(request, data);
    connect(reply, &QNetworkReply::finished, this, &LycheeUtils::logoutFinished);
    connect(reply, &QNetworkReply::sslErrors, this, &LycheeUtils::sslErrors);
}

void LycheeUtils::logoutFinished()
{
    QByteArray data = reply->readAll();
    QString s(data);
    debugPrint(s);

    if(setError()) return;

    if(s == "true")
    {
        // do nothing in logout no matter the result is
    }

    reply->deleteLater();
    reply = Q_NULLPTR;
}

void LycheeUtils::sslErrors(const QList<QSslError> &errors)
{
    reply->ignoreSslErrors();
}

bool LycheeUtils::setError(bool forceError)
{
    if(forceError || reply->error() != QNetworkReply::NoError)
    {
        if(reply != Q_NULLPTR)
        {
            reply->deleteLater();
        }
        reply = Q_NULLPTR;

        emit finished(this, "");

        return true;
    }

    return false;
}

void LycheeUtils::debugPrint(QString r)
{
#ifdef QT_DEBUG
    QString error = reply->errorString();
    if(reply->error() != QNetworkReply::NoError)
    {
        qInfo() << "Error occured : " << error << endl;
    }
    qInfo() << "request header\n";
    foreach (QByteArray h, reply->request().rawHeaderList()) {
        qInfo() << h << " : " << reply->request().rawHeader(h) << endl;
    }
    qInfo() << "reply header\n";
    foreach (QNetworkReply::RawHeaderPair p, reply->rawHeaderPairs()) {
        qInfo() << p.first << " : " << p.second << endl;
    }
    qInfo() << "reply data\n";
    qInfo() << r << endl;
#endif
}
