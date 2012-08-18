#pragma once
#include "QCommandLine/QCommandLine.h"
#include "StarlabSettings.h"

class PluginManager;

class CmdLineParser : public QObject{
    Q_OBJECT

private:
    QCommandLine parser;
    StarlabSettings* const settings;
   
public:
    /// Specify the structure here
    CmdLineParser(int argc, char *argv[], StarlabSettings* settings);
    
public slots:
    /// Errors result in app termination
    void parseError(const QString & name);
    /// i.e. --help, -h
    void switchFound(const QString & name);
    /// i.e. --filter="something"
    void optionFound(const QString & name, const QVariant & value);
    /// i.e. ~/Data/mesh.off 
    void paramFound(const QString & name, const QVariant & value);
};
