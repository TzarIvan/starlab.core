#pragma once
#include "Model.h"
#include "StarlabPlugin.h"
#include "RichParameterSet.h"

class FilterPlugin : public StarlabPlugin{
public:
    virtual void applyFilter(RichParameterSet*) = 0;
    virtual void initParameters(RichParameterSet*){}

protected:    
    using StarlabPlugin::model;
    using StarlabPlugin::document;
    using StarlabPlugin::drawArea;
};

Q_DECLARE_INTERFACE(FilterPlugin, "starlab.FilterPlugin/1.2")
