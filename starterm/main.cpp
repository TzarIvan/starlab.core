#include <QCoreApplication>
#include "CmdLineparser.h"
#include "StarlabApplication.h"
#include "PluginManager.h"

#include "interfaces/FilterPlugin.h";

int main(int argc, char *argv[]){ 
    try{
        QCoreApplication* app = new QCoreApplication(argc, argv);
        QCoreApplication::setOrganizationName("Free Software Foundation");
        QCoreApplication::setApplicationName("starterm");
        QCoreApplication::setApplicationVersion(StarlabApplication::version());
        CmdLineParser* parser = new CmdLineParser(argc,argv,app);
        StarlabApplication* starlab = new StarlabApplication();
        Document* document = starlab->document();
        
        /// Print a bunch of examples to command line
        if(parser->showExamples){
            qDebug("starterm --help");
            qDebug("starterm --list-filters");
            qDebug("starterm --list-filters %s/data/sphere.pts ", qPrintable(starlab->starlabDirectory()));
            return 0;
        }
        
        /// Requested ALL filter list regardless of input
        if(parser->listFilters && parser->inputModels.isEmpty()){
            qDebug() << "Available filters (Model=Any): ";
            foreach(FilterPlugin* filter, starlab->applicableFilters())
                qDebug(" -->  %s \t %s", qPrintable(filter->name()), qPrintable(filter->description()));
            return 0;
        }
    
        /// Load models in the document
        if(!parser->inputModels.isEmpty()){
            qDebug() << "Loading models into the document";
            foreach(QString path, parser->inputModels)
                starlab->load(path);
        }
        
        /// Requested filters appropriate to given input
        if(parser->listFilters && !document->models().size()){
            Model* model = document->selectedModel();
            qDebug("Available filters (Model:%s):", qPrintable(model->metaObject()->className()));
            foreach(FilterPlugin* filter, starlab->applicableFilters(model))
                qDebug(" -->  %s \t %s", qPrintable(filter->name()), qPrintable(filter->description()));
        }
        
        /// Executes the filter
        if(!parser->executeFilter.isEmpty() && document->models().size()>0 ){
            qDebug("Executing the filter '%s'", qPrintable(parser->executeFilter));
            starlab->executeFilter(parser->executeFilter);
        }

        /// Saves results in a new file
        if(parser->saveCreatecopy && document->models().size()>0){
            qDebug() << "[TODO] Saving filtered models (Safe Copy)";
        }
        if(parser->saveOverwrite && document->models().size()>0){
            qDebug("Saving filtered models (Overwriting)");
            foreach(Model* model, document->models())
                starlab->saveModel(model,model->path);
        }
    } 
    STARLAB_CATCH_BLOCK_NOGUI 
    return 0;                                
}
