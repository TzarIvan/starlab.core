#pragma once
#include <QCoreApplication>
#include "StarlabApplication.h"

class CoreApplication : public QCoreApplication{
public:
    CoreApplication(int &argc, char **argv) :
        QCoreApplication(argc, argv){
        setOrganizationName("Free Software Foundation");
        setApplicationName("starterm");
        setApplicationVersion("1.0.1");    
    }
};
