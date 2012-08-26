#pragma once

#include "interfaces/ProjectInputOutputPlugin.h"
class StarlabApplication;

class project_io_starlab : public ProjectInputOutputPlugin{
    Q_OBJECT
    Q_INTERFACES(ProjectInputOutputPlugin)

public:
    QString name() { return "Starlab Project (*.starlab)"; }
    virtual bool open(const QString& path, StarlabApplication& app);
    virtual void save(const QString& path, Document& m);
};