#include "CmdLineParser.h"
#include "PluginManager.h"
#include "StarlabApplication.h"

#include <QDebug>
#include <QCoreApplication>
#include <QVariant>

CmdLineParser::CmdLineParser(int argc, char *argv[], StarlabSettings* _settings) : 
    settings( _settings )
{
    parser.setArguments(argc, argv);
    
    /// Real options
    parser.enableVersion(true); ///< enable -v // --version
    parser.enableHelp(true); ///< enable -h / --help
    parser.addParam("model", "The set models to load", QCommandLine::Multiple);
    
    /// Filter options 
    parser.addSwitch(QChar::Null, "execute", "should we execute the filter or only open the dialog?");
    parser.addOption(QChar::Null, "filter", "Runs the specified filter, to show available filters type \"starlab --show-filters\"", QCommandLine::Multiple);
    parser.addSwitch(QChar::Null, "list-filters", "Shows available filters");
        
    /// Set this class as the parser
    connect(&parser, SIGNAL(switchFound(const QString &)), this, SLOT(switchFound(const QString &)));
    connect(&parser, SIGNAL(optionFound(const QString &, const QVariant &)), this, SLOT(optionFound(const QString &, const QVariant &)));
    connect(&parser, SIGNAL(paramFound(const QString &, const QVariant &)), this, SLOT(paramFound(const QString &, const QVariant &)));
    connect(&parser, SIGNAL(parseError(const QString &)), this, SLOT(parseError(const QString &)));
    
    /// Default hardcoded options
    parser.parse();
}

void CmdLineParser::parseError(const QString& error){
    qWarning() << qPrintable(error);
    parser.showHelp(true, -1);
    exit(0);
}

void CmdLineParser::switchFound(const QString & name){
    // qWarning() << "Switch:" << name;
    if(name=="list-filters") settings->set("action","list-filters");        
    if(name=="execute")      settings->set("action","execute");
}

void CmdLineParser::optionFound(const QString & name, const QVariant & value){
    // qWarning() << "Option:" << name << value.toString();
    if(name=="filter") settings->set("filter", value.toString());
}

void CmdLineParser::paramFound(const QString& /*name*/, const QVariant & value){
    // qWarning() << "Parameter:" << name << value.toString();
    QStringList pars = settings->getStringList("inputs");
    pars.append(value.toString());
    settings->set("inputs",pars);
}
