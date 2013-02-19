#include <QColor>
#include <QDir>
#include <QApplication>
#include "OSQuery.h"
#include "StarlabSettings.h"
#include "StarlabException.h"

using namespace Starlab;

StarlabSettings::StarlabSettings(StarlabApplication* application) :
    _application(application)
{
    /// Open and reads the settings
    qsettings = new QSettings(settingsFilePath(),QSettings::IniFormat);
    
    /// Defaults   
    setDefault("lastUsedDirectory",QDir::homePath());
    setDefault("autostartWithAction","");
}

QString StarlabSettings::settingsFilePath(){
    /// Attempt to use local file
    QString path = _application->executionDirectory().absoluteFilePath("settings.ini");
    QFileInfo fi(path);
    if( fi.exists() && fi.isReadable() )
        return path;
    
    /// Otherwise load system file
    if(OSQuery::isMac() || OSQuery::isWin()) 
        return _application->starlabDirectory().absoluteFilePath("settings.ini");
    if(OSQuery::isLinux()) 
        return QDir::homePath() + ".starlab";
    Q_ASSERT(false);
    return "";
}

/// Display only settings in "starlab" group
QByteArray StarlabSettings::toLocal8Bit(){
    qsettings->beginGroup("starlab");    
        QString retval="";
        retval += "========== Starlab Settings: =========\n";
        foreach(QString key, qsettings->allKeys())
            retval += key + ": (" + qsettings->value(key).toString() + ")\n";
        retval += "======================================\n";
    qsettings->endGroup();
    return retval.toLocal8Bit();
}

QVariant StarlabSettings::get(const QString &key){
    checkContains(key);
    return qsettings->value("starlab/"+key);
}

void StarlabSettings::set(const QString &key, const QVariant &value){
    qsettings->setValue("starlab/"+key, value);   
}

void StarlabSettings::setDefault(const QString &key, const QVariant &value){
    if(!contains(key)) set(key, value);
}

bool StarlabSettings::contains(const QString &key){ 
    return qsettings->contains("starlab/"+key);
}

void StarlabSettings::checkContains(const QString &key){
    if(!contains(key))
        qWarning() << "CRITICAL: Requested unexisting setting with key: " << key;
}

float* StarlabSettings::getColor4fv(const QString& key){
    const QVariant& setting = get(key);
    static float a[4];
    QColor c = setting.value<QColor>();
    a[0] = c.redF();
    a[1] = c.greenF();
    a[2] = c.blueF();
    a[3] = c.alphaF();
    return a;
}

bool StarlabSettings::getBool(const QString &key){
    return get(key).toBool();
}

QString StarlabSettings::getString(const QString &key){ return get(key).toString(); }
QStringList StarlabSettings::getStringList(const QString &key){ return get(key).toStringList(); }
QColor StarlabSettings::getQColor(const QString &key){ return get(key).value<QColor>(); }
float StarlabSettings::getFloat(const QString &key){ return get(key).toFloat(); }
