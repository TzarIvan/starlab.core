#pragma once
#include <QSettings>
#include <QDebug>
#include <QColor>

#include "dynamic_common_global.h"
#include "StarlabApplication.h"

class DYNAMIC_COMMON_EXPORT StarlabSettings{
    
private: 
    QSettings* qsettings;
    StarlabApplication* const _application;
public:
    /**
      * The constructor first set some hardcoded parameters, 
      * then attempts to override their values with the ones
      * specified by the xml options file 
      */
    StarlabSettings(StarlabApplication *application);
    void sync(){ qsettings->sync(); }
    
    /// Sets hardcoded default setting value (writes only if value is not already defined)
    void setDefault(const QString& key, const QVariant& value);
    void set(const QString& key, const QVariant& value);

    QVariant get(const QString& key);
    bool contains(const QString& key);
    void checkContains(const QString& key);
    
    /// Simplified / Specialized converters
    /// @{ @name Type-Specialized getters    
    public:
        bool getBool(const QString& key);
        float getFloat(const QString& key);
        float* getColor4fv(const QString& key);     
        QColor getQColor(const QString& key);
        QString getString(const QString& key);
        QStringList getStringList(const QString& key);
    /// @}

        
    /// @{ Settings I/O
        /// Where will the "ini" file be saved?
        QString settingsFilePath();
        /// @todo load settings from file
        void load(const QString& filename="");
        /// @todo save settings to file
        void save(const QString& filename="");
    /// @}
    
    
    /// Print to string (e.g. qDebug() << this->toString() )
    QByteArray toLocal8Bit();
};
