/*
 * Better Chat plugin for TeamSpeak 3
 * GPLv3 license
 *
 * Copyright (C) 2019 Luch (https://github.com/Luch00)
*/

#include "ConfigWidget.h"
#include <QLabel>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFileDialog>
#include <QCryptographicHash>
#include <QRandomGenerator>

ConfigWidget::ConfigWidget(const QString& path, QWidget *parent)
	: QWidget(parent)
	, configPath(QString("%1LxBTSC/template/config.json").arg(path))
{
	this->setWindowTitle("Better Chat Settings");
	generalTab = new QWidget();
	formLayout = new QFormLayout(generalTab);
	generalTab->setLayout(formLayout);
	embeds = new QCheckBox("Enable embeds", this);
	embeds->setChecked(true);
	generics = new QCheckBox("Disable generic embeds", this);
	generics->setChecked(false);
	favicons = new QCheckBox("Enable favicons for links", this);
	favicons->setChecked(false);
	emoticons = new QCheckBox("Enable emotes", this);
	emoticons->setChecked(true);
	avatar = new QCheckBox("Avatars in chat", this);
	avatar->setChecked(false);
	stopGifs = new QCheckBox("Play gifs only on hover", this);
	stopGifs->setChecked(false);
	history = new QCheckBox("Load chat history", this);
	history->setChecked(false);
	maxlines = new QSpinBox(this);
	maxlines->setMinimum(50);
	maxlines->setMaximum(1000);
	maxlines->setValue(500);
	maxHistory = new QSpinBox(this);
	maxHistory->setMinimum(10);
	maxHistory->setMaximum(500);
	maxHistory->setValue(50);
	fontsize = new QSpinBox(this);
	fontsize->setMinimum(6);
	fontsize->setMaximum(34);
	fontsize->setValue(12);
	downloadDir = new QLineEdit("", this);
	downloadDir->setDisabled(true);
	QPushButton* browseButton = new QPushButton("...", this);
	browseButton->setToolTip("Browse folder");
	browseButton->setFixedWidth(40);
	remotes = new QPlainTextEdit(this);
	remotes->setMaximumHeight(70);
	remotes->setLineWrapMode(QPlainTextEdit::WidgetWidth);
	remotes->setPlaceholderText("Separate multiple urls with '|'");

	eventTab = new QWidget();
	eventLayout = new QFormLayout(eventTab);
	eventTab->setLayout(eventLayout);
	kickEvent = new QCheckBox("Show kicks");
	banEvent = new QCheckBox("Show bans");
	moveSelfEvent = new QCheckBox("Show own move");
	moveOtherEvent = new QCheckBox("Show others move");
	channelCreatedEvent = new QCheckBox("Show channel created");
	channelDeletedEvent = new QCheckBox("Show channel deleted");
	clientConnectedEvent = new QCheckBox("Show client connect");
	clientDisconnectedEvent = new QCheckBox("Show client disconnect");
	ownDisconnectedEvent = new QCheckBox("Show own disconnect");
	ownConnectedEvent = new QCheckBox("Show own connect");

	imageTab = new QWidget();
	imageLayout = new QFormLayout(imageTab);
	imageTab->setLayout(imageLayout);
	galleryUrl = new QLineEdit();
	galleryUserName = new QLineEdit();
	galleryPassword = new QLineEdit();
	galleryPassword->setEchoMode(QLineEdit::Password);
	galleryPassword->setMaxLength(64);

	imageLayout->addRow(new QLabel("Gallery Url:", this), galleryUrl);
	imageLayout->addRow(new QLabel("User name:", this), galleryUserName);
	imageLayout->addRow(new QLabel("Password:", this), galleryPassword);

	eventLayout->addRow(kickEvent);
	eventLayout->addRow(banEvent);
	eventLayout->addRow(moveSelfEvent);
	eventLayout->addRow(moveOtherEvent);
	eventLayout->addRow(channelCreatedEvent);
	eventLayout->addRow(channelDeletedEvent);
	eventLayout->addRow(clientConnectedEvent);
	eventLayout->addRow(clientDisconnectedEvent);
	eventLayout->addRow(ownConnectedEvent);
	eventLayout->addRow(ownDisconnectedEvent);

	saveButton = new QPushButton("Save", this);
	connect(saveButton, &QPushButton::clicked, this, &ConfigWidget::save);
	connect(browseButton, &QPushButton::clicked, this, &ConfigWidget::browseDirectory);

	QHBoxLayout* horizontal = new QHBoxLayout();
	horizontal->addSpacing(200);
	horizontal->addWidget(saveButton);
	formLayout->addRow(embeds);
	formLayout->addRow(generics);
	formLayout->addRow(favicons);
	formLayout->addRow(emoticons);
	formLayout->addRow(avatar);
	formLayout->addRow(stopGifs);
	formLayout->addRow(history);
	formLayout->addRow(new QLabel("Max lines in tab:", this), maxlines);
	formLayout->addRow(new QLabel("Max lines of history:", this), maxHistory);
	formLayout->addRow(new QLabel("Font size:", this), fontsize);
	formLayout->addRow(new QLabel("Download directory:"));
	formLayout->addRow(downloadDir);
	formLayout->addRow(browseButton);
	formLayout->addRow(new QLabel("Remote emote definitions:"));
	formLayout->addRow(remotes);
	formLayout->addItem(new QSpacerItem(0, 10));
	tabWidget = new QTabWidget(this);
	tabWidget->addTab(generalTab, "General");
	tabWidget->addTab(eventTab, "Events");
	tabWidget->addTab(imageTab, "Image");
	configLayout = new QFormLayout(this);
	this->setLayout(configLayout);
	configLayout->addRow(tabWidget);
	configLayout->addRow(horizontal);

	readConfig();
}

ConfigWidget::~ConfigWidget()
{
}

void ConfigWidget::readConfig()
{
	QFile file(configPath);
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QString jsonText = file.readAll();
		file.close();
		QJsonDocument document = QJsonDocument::fromJson(jsonText.toUtf8());
		jsonObj = document.object();
		embeds->setChecked(jsonObj.value("EMBED_ENABLED").toBool(true));
		generics->setChecked(jsonObj.value("GENERICS_DISABLED").toBool(false));
		favicons->setChecked(jsonObj.value("FAVICONS_ENABLED").toBool(false));
		emoticons->setChecked(jsonObj.value("EMOTICONS_ENABLED").toBool(true));
		avatar->setChecked(jsonObj.value("AVATARS_ENABLED").toBool(false));
		stopGifs->setChecked(jsonObj.value("HOVER_ANIMATES_GIFS").toBool(false));
		history->setChecked(jsonObj.value("HISTORY_ENABLED").toBool(false));
		maxlines->setValue(jsonObj.value("MAX_LINES").toInt(500));
		maxHistory->setValue(jsonObj.value("MAX_HISTORY").toInt(50));
		fontsize->setValue(jsonObj.value("FONT_SIZE").toInt());
		downloadDir->setText(jsonObj.value("DOWNLOAD_DIR").toString());

		kickEvent->setChecked(jsonObj.value("EVENT_KICK").toBool(true));
		banEvent->setChecked(jsonObj.value("EVENT_BAN").toBool(true));
		moveSelfEvent->setChecked(jsonObj.value("EVENT_MOVESELF").toBool());
		moveOtherEvent->setChecked(jsonObj.value("EVENT_MOVEOTHER").toBool());
		channelCreatedEvent->setChecked(jsonObj.value("EVENT_CHANNELCREATE").toBool());
		channelDeletedEvent->setChecked(jsonObj.value("EVENT_CHANNELDELETE").toBool());
		clientConnectedEvent->setChecked(jsonObj.value("EVENT_CLIENTCONNECT").toBool(true));
		clientDisconnectedEvent->setChecked(jsonObj.value("EVENT_CLIENTDISCONNECT").toBool(true));
		ownConnectedEvent->setChecked(jsonObj.value("EVENT_SELFCONNECT").toBool(true));
		ownDisconnectedEvent->setChecked(jsonObj.value("EVENT_SELFDISCONNECT").toBool(true));

		galleryUrl->setText(jsonObj.value("IMAGE_URL").toString());
		galleryUserName->setText(jsonObj.value("IMAGE_USERNAME").toString());
		QByteArray ePassword = QByteArray::fromBase64(jsonObj.value("IMAGE_PASSWORD").toString().toLocal8Bit());
		QByteArray salt = ePassword.right(8);
		ePassword.truncate(128);
		QByteArray hash = ePassword.right(64);
		QByteArray password;
		for (int i = 0; i < 64; i++)
		{
			password.append(ePassword[i] ^ hash[i]);
		}
		for (; password.length(); password.remove(password.length()-1, 1))
		{
			if(QCryptographicHash::hash(password + salt, QCryptographicHash::Sha512) == hash)
			{
				break;
			}
		}
		galleryPassword->setText(QString(password));

		QJsonArray remotejson = jsonObj.value("REMOTE_EMOTES").toArray();
		QStringList list;
		foreach(const QJsonValue &v, remotejson) 
		{
			list.append(v.toString());
		}
		remotes->setPlainText(list.join('|'));
	}
	else
	{
		// defaults when config file doesn't exist yet
		embeds->setChecked(true);
		generics->setChecked(false);
		favicons->setChecked(false);
		emoticons->setChecked(true);
		avatar->setChecked(false);
		stopGifs->setChecked(false);
		history->setChecked(false);
		maxlines->setValue(500);
		maxHistory->setValue(50);
		fontsize->setValue(12);
		downloadDir->setText("");

		kickEvent->setChecked(true);
		banEvent->setChecked(true);
		moveSelfEvent->setChecked(false);
		moveOtherEvent->setChecked(false);
		channelCreatedEvent->setChecked(false);
		channelDeletedEvent->setChecked(false);
		clientConnectedEvent->setChecked(true);
		clientDisconnectedEvent->setChecked(true);
		ownConnectedEvent->setChecked(true);
		ownDisconnectedEvent->setChecked(true);
	}
}

void ConfigWidget::browseDirectory()
{
	QString dir = QFileDialog::getExistingDirectory(this, "Select Directory", "", QFileDialog::ShowDirsOnly);
	downloadDir->setText(dir);
}

void ConfigWidget::save()
{
	jsonObj.insert("EMBED_ENABLED", embeds->isChecked());
	jsonObj.insert("GENERICS_DISABLED", generics->isChecked());
	jsonObj.insert("FAVICONS_ENABLED", favicons->isChecked());
	jsonObj.insert("EMOTICONS_ENABLED", emoticons->isChecked());
	jsonObj.insert("AVATARS_ENABLED", avatar->isChecked());
	jsonObj.insert("HOVER_ANIMATES_GIFS", stopGifs->isChecked());
	jsonObj.insert("HISTORY_ENABLED", history->isChecked());
	jsonObj.insert("MAX_LINES", maxlines->value());
	jsonObj.insert("MAX_HISTORY", maxHistory->value());
	jsonObj.insert("FONT_SIZE", fontsize->value());
	jsonObj.insert("DOWNLOAD_DIR", downloadDir->text());

	jsonObj.insert("EVENT_KICK", kickEvent->isChecked());
	jsonObj.insert("EVENT_BAN", banEvent->isChecked());
	jsonObj.insert("EVENT_MOVESELF", moveSelfEvent->isChecked());
	jsonObj.insert("EVENT_MOVEOTHER", moveOtherEvent->isChecked());
	jsonObj.insert("EVENT_CHANNELCREATE", channelCreatedEvent->isChecked());
	jsonObj.insert("EVENT_CHANNELDELETE", channelDeletedEvent->isChecked());
	jsonObj.insert("EVENT_CLIENTCONNECT", clientConnectedEvent->isChecked());
	jsonObj.insert("EVENT_CLIENTDISCONNECT", clientDisconnectedEvent->isChecked());
	jsonObj.insert("EVENT_SELFCONNECT", ownConnectedEvent->isChecked());
	jsonObj.insert("EVENT_SELFDISCONNECT", ownDisconnectedEvent->isChecked());

	jsonObj.insert("IMAGE_URL", galleryUrl->text());
	jsonObj.insert("IMAGE_USERNAME", galleryUserName->text());
	// it is NOT encrypt here, your password is NOT safe/secure
	QByteArray salt;
	QDataStream(&salt,QIODevice::WriteOnly) << QRandomGenerator::system()->generate64();
	QByteArray hash = QCryptographicHash::hash(galleryPassword->text().toLocal8Bit() + salt, QCryptographicHash::Sha512);
	QByteArray oPassword = galleryPassword->text().toLocal8Bit();
	QByteArray password;
	for (int i = 0; i < 64; i++)
	{
		if(i < oPassword.length())
		{
			password.append(oPassword[i] ^ hash[i]);
		}
		else
		{
			password.append(0xFF ^ hash[i]);
		}
	}
	password.append(hash);
	password.append(salt);
	jsonObj.insert("IMAGE_PASSWORD", QString(password.toBase64()));

	if (remotes->toPlainText().length() > 1)
	{
		jsonObj.insert("REMOTE_EMOTES", QJsonArray::fromStringList(remotes->toPlainText().split('|')));
	}
	else
	{
		jsonObj.insert("REMOTE_EMOTES", QJsonArray());
	}
	
	QFile file(configPath);
	if (file.open(QIODevice::WriteOnly))
	{
		QJsonDocument doc(jsonObj);
		file.write(doc.toJson());
		file.close();
		this->close();
		emit configChanged();
	}
}

QString ConfigWidget::getConfigAsString(const QString& key)
{
	return jsonObj.value(key).toString();
}

bool ConfigWidget::getConfigAsBool(const QString& key)
{
	return jsonObj.value(key).toBool();
}
