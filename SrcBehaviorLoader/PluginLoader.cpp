#include "PluginLoader.h"
#include <QDebug>
#include <vector>
#include <filesystem>


#ifdef _WIN32
#include <stdio.h>
#include <tchar.h>
#include <string.h>
#include <atlbase.h>
#include <QDebug>
#include <QFileInfo>
#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383
 
std::vector<std::string> QueryKey(HKEY hKey, std::string path) 
{ 
	//See https://docs.microsoft.com/en-us/windows/desktop/sysinfo/enumerating-registry-subkeys
	std::vector<std::string> list;
    TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
    DWORD    cchClassName = MAX_PATH;  // size of class string 
    DWORD    cSubKeys=0;               // number of subkeys 
    DWORD    cbMaxSubKey;              // longest subkey size 
    DWORD    cchMaxClass;              // longest class string 
    DWORD    cValues;              // number of values for key 
    DWORD    cchMaxValue;          // longest value name 
    DWORD    cbMaxValueData;       // longest value data 
    DWORD    cbSecurityDescriptor; // size of security descriptor 
    FILETIME ftLastWriteTime;      // last write time 
 
    DWORD i, retCode; 
 
    TCHAR  achValue[MAX_VALUE_NAME]; 
    DWORD cchValue = MAX_VALUE_NAME; 
 
    // Get the class name and the value count. 
    retCode = RegQueryInfoKey(
        hKey,                    // key handle 
        achClass,                // buffer for class name 
        &cchClassName,           // size of class string 
        NULL,                    // reserved 
        &cSubKeys,               // number of subkeys 
        &cbMaxSubKey,            // longest subkey size 
        &cchMaxClass,            // longest class string 
        &cValues,                // number of values for this key 
        &cchMaxValue,            // longest value name 
        &cbMaxValueData,         // longest value data 
        &cbSecurityDescriptor,   // security descriptor 
        &ftLastWriteTime);       // last write time 
 
    // Enumerate the key values. 
    if (cValues) 
    {
       	//printf( "\nNumber of values: %d\n", cValues);

        for (i=0, retCode=ERROR_SUCCESS; i<cValues; i++) 
        { 
            cchValue = MAX_VALUE_NAME; 
            achValue[0] = '\0'; 
            retCode = RegEnumValue(hKey, i, 
                achValue, 
                &cchValue, 
                NULL, 
                NULL,
                NULL,
                NULL);
 
            if (retCode == ERROR_SUCCESS ) 
            { 
				CRegKey regKey;
				CHAR szBuffer[512];
				ULONG dwBufferSize = sizeof(szBuffer);

				if(ERROR_SUCCESS != regKey.Open(HKEY_LOCAL_MACHINE, path.c_str()))
				{
					qWarning() << "Error opening registry path " << path.c_str(); 
					regKey.Close();
				}
				if( ERROR_SUCCESS != regKey.QueryStringValue(achValue,szBuffer,&dwBufferSize))
				{
					qWarning() << "Error opening registry value " << achValue;
					regKey.Close();
				}

				std::string fp = szBuffer;
				std::replace( fp.begin(), fp.end(), '\\', '/');
				list.push_back(fp);
            } 
        }
    }
	return list;
}
#endif

std::vector<std::string> PluginLoader::queryRegistryBehaviors(std::string path)
{
	std::vector<std::string> list;
	
	#ifdef _WIN32
	HKEY hTestKey;

	if( RegOpenKeyEx( HKEY_LOCAL_MACHINE,
		TEXT(path.c_str()),
		0,
		KEY_READ,
		&hTestKey) == ERROR_SUCCESS)
	{
		list = QueryKey(hTestKey, path);
	}

	RegCloseKey(hTestKey);
	#endif

	return list;
}

#ifdef _WIN32
const char * WinGetEnv(const char * name)
{
    const DWORD buffSize = 65535;
    static char buffer[buffSize];
    if (GetEnvironmentVariableA(name, buffer, buffSize))
    {
        return buffer;
    }
    else
    {
        return 0;
    }
}
bool WinSetEnv(const char* name, const char* toWhat){
	return SetEnvironmentVariableA(name, toWhat);
}
#endif

const char* PluginLoader::addDllPath(std::string f)
{
	//Get the directory of the DLL/*.so and add it to the PATH env variable.
	//This way dependencies can be shipped in the same directory
	#ifdef _WIN32
		QFileInfo finf(f.c_str());
		//rather than the buggy _getenv: https://docs.microsoft.com/de-de/windows/desktop/api/winbase/nf-winbase-getenvironmentvariable
		auto old_path = WinGetEnv("PATH");
		auto path = std::ostringstream();
		if(old_path){
			path << old_path << ";" << finf.absolutePath().toStdString().c_str();
			WinSetEnv("PATH", path.str().c_str());
		}else{
			qWarning() << "Failed to get and modify PATH enviromental variable.";
		}
		return old_path;
	#endif
	
	return "";
}

void PluginLoader::delDllPath(const char* oldPath){
	//reset path. We don't want some weird cross-effects
	#ifdef _WIN32
		if(oldPath){
			WinSetEnv("PATH", oldPath);
		}
	#endif
}

bool endsWith(std::string value, std::string ending)
{
	std::transform(value.begin(), value.end(), value.begin(), ::tolower);
	std::transform(ending.begin(), ending.end(), ending.begin(), ::tolower);
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

std::vector<std::string> PluginLoader::searchDirectoriesForPlugins(std::vector<std::string> list){
	//Search directories
    std::vector<std::string> filesFromFolders;

    for (auto f: list) {
        std::string file = f;
        try {
            if (!file.empty() && file[file.size() - 1] == '/') {
                for (auto& p : std::filesystem::directory_iterator(file)) {
					std::string s = p.path().string();
					if(endsWith(s,".robo_tracker.dll") || endsWith(s,".robo_tracker.so"))
                    	filesFromFolders.push_back(s);
                }
            }
            else {
				if(endsWith(f,".robo_tracker.dll") || endsWith(f,".robo_tracker.so"))
                	filesFromFolders.push_back(f);
            }
        }
        catch (...){
            qWarning() << "Could not read file/directory: " << file.c_str();
        }
    }
	
	return filesFromFolders;
}

PluginLoader::PluginLoader(QObject *parent)
{
	m_PluginLoader = new QPluginLoader(this);
}

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

QJsonObject PluginLoader::getPluginMetaData() const
{
	return m_MetaData;
}

void PluginLoader::readMetaDataFromPlugin()
{
	m_MetaData = m_PluginLoader->metaData().value("MetaData").toObject();
}
