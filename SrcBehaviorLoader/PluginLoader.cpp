#include "PluginLoader.h"

#include <QDebug>

PluginLoader::PluginLoader(QObject *parent)
{
	m_PluginLoader = new QPluginLoader(this);
}

/*---------------------------------------*/
bool PluginLoader::loadPluginFromFilename(QString const& filename)
{
	bool retval = false;
	if (m_PluginLoader->isLoaded()) {

		m_PluginLoader->unload();
	}

	bool isLib = QLibrary::isLibrary(filename);

	if (isLib) {

		m_PluginLoader->setFileName(filename);

		readMetaDataFromPlugin();

		retval = m_PluginLoader->load();
		QString s = m_PluginLoader->errorString();
		std::string ss = s.toStdString();

    if (!m_PluginLoader->isLoaded())
		{
		    qWarning() << ss.c_str();
		}
	}
	else {
		retval = false;
	}

	return retval;
}

/*---------------------------------------*/
IBehaviourFactory* PluginLoader::getPluginInstance()
{
	return qobject_cast<IBehaviourFactory*>(m_PluginLoader->instance());
}

/*---------------------------------------*/
QJsonObject PluginLoader::getPluginMetaData() const
{
	return m_MetaData;
}

/*---------------------------------------*/
void PluginLoader::readMetaDataFromPlugin()
{
	m_MetaData = m_PluginLoader->metaData().value("MetaData").toObject();
}
