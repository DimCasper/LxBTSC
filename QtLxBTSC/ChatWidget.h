/*
* Teamspeak 3 chat plugin
* HTML chatbox using WebEngine
*
* Copyright (c) 2017 Luch
*/

#pragma once

#include <QWidget>
#include <QWebEngineView>
#include <QWebEngineSettings>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <TsWebEnginePage.h>
#include <QClipboard>
//#include <QGuiApplication>
#include <QMenu>
#include <QtWebChannel/qwebchannel.h>
#include <TsWebObject.h>
#include <QWebEngineFullScreenRequest>
#include <QApplication>
#include <QKeyEvent>
#include "FullScreenWindow.h"

class ChatWidget : public QFrame
{
	Q_OBJECT

public:
	ChatWidget(QString path, TsWebObject* webObject, QWidget *parent = Q_NULLPTR);
	~ChatWidget();
	void reload() const;

	signals:
	void fileUrlClicked(const QUrl &url);
	void clientUrlClicked(const QUrl &url);
	void channelUrlClicked(const QUrl &url);
	void linkHovered(const QUrl &url);
	void chatLoaded();

	private slots:
	void onCopyActivated();
	void onCopyUrlActivated();
	void onShowContextMenu(const QPoint &);
	void onLinkHovered(const QUrl &);
	void onFullScreenRequested(QWebEngineFullScreenRequest request);

private:
	QVBoxLayout *verticalLayout;
	QWebEngineView *view;
	QScopedPointer<FullScreenWindow> fullScreenWindow;
	TsWebObject *wObject;
	TsWebEnginePage *page;
	QString pathToPage;
	QUrl currentHoveredUrl;
	QMenu * menu;
	QAction *copyAction;
	QAction *copyUrlAction;
	QWebChannel *channel;
	void setupUi();
	void createPage();
	void keyReleaseEvent(QKeyEvent* event) override;
};
