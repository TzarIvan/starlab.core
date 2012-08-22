/// In this file we specialize the generic plugins to act on a specific model
/// Note this is not strictly necessary, but it allows us to group a lot of 
/// checks and casts that are necessary in any plugin.

#pragma once
#include "CloudModel.h"
#include "interfaces/InputOutputPlugin.h"
#include "interfaces/RenderPlugin.h"
#include "interfaces/FilterPlugin.h"
#include "interfaces/ModePlugin.h"

/// This namescape limits the visibility of these functions to this file
namespace{
    bool isA(Model* model){
        return qobject_cast<CloudModel*>(model);
    }
    CloudModel* safeCast(Model* model){
        CloudModel* cloud = qobject_cast<CloudModel*>(model);
        if(!cloud) throw StarlabException("Model is not a SurfaceMeshModel");
        return cloud;
    }
}

class CloudInputOutputPlugin : public InputOutputPlugin{
private: 
    bool isApplicable(Model* model){ return isA(model); }
};

class CloudFilterPlugin : public FilterPlugin{
private:
    bool isApplicable(Model* model){ return isA(model); }    
};

class CloudModePlugin : public ModePlugin{
private:
    bool isApplicable(Document* doc){ return isA(doc->selectedModel()); }    
};

class CloudRenderPlugin : public RenderPlugin{
private:
    bool isApplicable(Model* model){ return isA(model); }
    CloudModel* cloud(){ return safeCast(model()); }
};
