/*
* Teamspeak 3 chat plugin
* HTML chatbox using WebEngine
*
* Copyright (c) 2017 Luch
*/

#pragma once

#include <QWidget>
#include <QtWebEngineWidgets/QWebEngineView>
#include <QtWebEngineWidgets/qwebenginesettings.h>
#include <QtWidgets/QVBoxLayout>
#include <QStackedWidget>
#include <TsWebEnginePage.h>
#include <QShortcut>
#include <QClipboard>
#include <QGuiApplication>
#include <QMenu>
#include <QtWebChannel/qwebchannel.h>
#include <TsWebObject.h>

class ChatWidget : public QFrame
{
	Q_OBJECT

public:
	ChatWidget(QString path, QWidget *parent = Q_NULLPTR);
	~ChatWidget();
	void createPage();

	TsWebObject* ChatWidget::webObject() const
	{
		return wObject;
	}

	signals:
	void fileUrlClicked(const QUrl &url);

	private slots:
	void copyActivated();
	void copyUrlActivated();
	void showContextMenu(const QPoint &);
	void linkHovered(const QUrl &);
	void onFileUrlClicked(const QUrl &url);

private:
	QVBoxLayout *verticalLayout;
	QWebEngineView *view;
	TsWebObject *wObject;
	TsWebEnginePage *page;
	void setupUi(QWidget *ChatWidget);
	QString pathToPage;
	QUrl currentHoveredUrl;
	QMenu * menu;
	QShortcut *copy;
	QAction *copyAction;
	QAction *copyUrlAction;
	QWebChannel *channel;
};
