#pragma once
#include "Model.h"
#include "StarlabPlugin.h"

class RichParameterSet;

class FilterPlugin : public StarlabPlugin{
public:
    virtual void applyFilter(RichParameterSet*) = 0;
    virtual void initParameters(RichParameterSet*){}
    virtual bool isApplicable(Model* model) = 0;

    /// @{ access to resources
    protected:
        using StarlabPlugin::document;
        using StarlabPlugin::drawArea;
        Model* model(){ return document()->selectedModel(); }    
    /// @}
};

Q_DECLARE_INTERFACE(FilterPlugin, "starlab.FilterPlugin/1.2")