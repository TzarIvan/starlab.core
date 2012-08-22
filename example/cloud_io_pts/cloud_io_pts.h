#pragma once
#include "CloudPlugins.h"
class cloud_io_pts : public CloudInputOutputPlugin{
    Q_OBJECT
    Q_INTERFACES(InputOutputPlugin)

public:
    QString name() { return "3D Point List (*.pts)"; }
    Model* open(QString path);
    void save(Model* m,QString path);
};
