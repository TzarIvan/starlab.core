#include "StarlabSettings.h"
#include <QColor>
#include <QDir>

StarlabSettings::StarlabSettings(){
    qsettings.sync();

    /// Defaults   
    setDefault("showtrackball",false);
    setDefault("lastUsedDirectory",QDir::homePath());
    setDefault("DefaultDrawAreaPlugin","drawarea_meshlab");
    
    /// Resets at every run
    set("nogui",false);           ///< should a window be opened?
    set("action","");             ///< the auto-loaded action (i.e. execute filter)
    set("filter","");             ///< the auto-loaded filter
    set("edit","");               ///< the auto-loaded edit 
    set("inputs",QStringList());  ///< the program inputs (command line)
        
    /// Now override any hardcoded with the ones specified by the user parameters
    load();
}

/// Display only settings in "starlab" group
QByteArray StarlabSettings::toLocal8Bit(){
    qsettings.beginGroup("starlab");    
        QString retval="";
        retval += "========== Starlab Settings: =========\n";
        foreach(QString key, qsettings.allKeys())
            retval += key + ": (" + qsettings.value(key).toString() + ")\n";
        retval += "======================================\n";
    qsettings.endGroup();
    return retval.toLocal8Bit();
}

QVariant StarlabSettings::get(const QString &key){
    checkContains(key);
    return qsettings.value("starlab/"+key);
}

void StarlabSettings::set(const QString &key, const QVariant &value){
    qsettings.setValue("starlab/"+key, value);   
}

void StarlabSettings::setDefault(const QString &key, const QVariant &value){
    if(!contains(key)) set(key, value);
}

bool StarlabSettings::contains(const QString &key){ 
    return qsettings.contains("starlab/"+key);
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
QColor StarlabSettings::getQColor(const QString &key){ return get(key).convert(QVariant::Color); }
float StarlabSettings::getFloat(const QString &key){ return get(key).toFloat(); }


void StarlabSettings::load(const QString& /*filename*/){ }
void StarlabSettings::save(const QString& /*filename*/){ }






