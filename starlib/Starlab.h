#pragma once

/** 
 * @defgroup starcore Starlab Core Classes
 * @brief The classes around which starlab is constructed.
 */ 

namespace Starlab{
    class Model;
    class Document;
    class DrawArea;
    class Application;
    class MainWindow;
    class Settings;
    class PluginManager;    
}

/// @todo Should RichParameterSet be inserted into Starlab::?
class RichParameterSet;


#include "Eigen/Dense"
namespace Starlab{
    typedef Eigen::Vector3d Vector3;
    typedef Eigen::Hyperplane<double,3> Plane3;
    typedef Eigen::ParametrizedLine<double,3> Ray3;
}
