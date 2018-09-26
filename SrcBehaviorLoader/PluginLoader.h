#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include "QPluginLoader"
#include "QStringListModel"
#include "IBehaviourFactory.h"
#include "QPointer"

class PluginLoader : QObject
{
	Q_OBJECT
public:
	explicit PluginLoader(QObject *parent = 0);

	bool loadPluginFromFilename(QString const& filename);
	IBehaviourFactory *getPluginInstance();
	QJsonObject getPluginMetaData() const;
private:

	void readMetaDataFromPlugin();

	QPluginLoader *m_PluginLoader;
	QJsonObject m_MetaData;
};

#endif // PLUGINLOADER_H
