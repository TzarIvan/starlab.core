#pragma once
#include "StarlabPlugin.h"

class Document;
class StarlabSettings;
class StarlabDrawArea;
class StarlabMainWindow;
class StarlabApplication;

/// Different rendering styles!!
class DrawAreaPlugin : public StarlabPlugin{
public:
    /// Loads draw area setup into the StarlabMainWindow
    /// Instantiate and return a StarlabDrawArea object
    virtual StarlabDrawArea* load() = 0;

protected:
    using StarlabPlugin::application;
    using StarlabPlugin::mainWindow;
    using StarlabPlugin::settings;
};

Q_DECLARE_INTERFACE(DrawAreaPlugin, "starlab.DrawAreaPlugin/1.0")
