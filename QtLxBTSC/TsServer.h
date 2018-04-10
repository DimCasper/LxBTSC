#pragma once

#include <QObject>
#include <QMap>
#include <QSharedPointer>
#include "TsClient.h"

class TsServer : public QObject
{
	Q_OBJECT

public:
	TsServer(QObject *parent = 0);
	TsServer(unsigned long long serverId, QString uniqueId, QMap<unsigned short, QSharedPointer<TsClient>> clients, QObject *parent = 0);
	TsServer(unsigned long long serverId, QString uniqueId, QObject *parent = 0);
	~TsServer();

	QString uniqueId() const;
	QString safeUniqueId() const;

	void addClients(QMap<unsigned short, QSharedPointer<TsClient>> newClients);
	void addClient(unsigned short clientId, QSharedPointer<TsClient>);
	QSharedPointer<TsClient> getClient(unsigned short clientId) const;
	QSharedPointer<TsClient> getClientByName(QString name) const;

	bool operator==(const TsServer* server) const
	{
		return this->uniqueId_ == server->uniqueId();
	}

private:
	unsigned long long serverId_;
	const QString uniqueId_;
	QString safeUniqueId_;
	QMap<unsigned short, QSharedPointer<TsClient>> clients_;
};
