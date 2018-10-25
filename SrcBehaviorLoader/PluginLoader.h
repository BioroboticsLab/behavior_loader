
#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include "QPluginLoader"
#include "QStringListModel"
#include "QPointer"

class PluginLoader : QObject
{ 
	Q_OBJECT
public:
	explicit PluginLoader(QObject *parent = 0);

	bool loadPluginFromFilename(QString const& filename);
	QJsonObject getPluginMetaData() const;

	static std::vector<std::string> queryRegistryBehaviors(std::string path);
	static std::vector<std::string> searchDirectoriesForPlugins(std::vector<std::string> list);

	//return oldPath
	static const char* addDllPath(std::string file);
	static void delDllPath(const char* oldPath);
private:

	void readMetaDataFromPlugin();

	QPluginLoader *m_PluginLoader;
	QJsonObject m_MetaData;
};

#endif // PLUGINLOADER_H