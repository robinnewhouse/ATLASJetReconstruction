#include "JSSTutorial/ToolConfigHelper.h"
#include "TPython.h"
#include <iostream>

IJetExecuteTool* ToolWrapper::init(const std::string & className, const std::string & toolName, const std::string & scriptName, const std::string & configCall ){

    TPython p;
    p.Bind(this, "wrapper");
    p.Exec("import ROOT");
    p.Exec("import JSSTutorial.RootCoreConfigInit");
    // Create the concrete instance on the python side
    std::string ctorCall = "tool=ROOT."+className;
    ctorCall+="('"; ctorCall+=toolName;   // ctor
    ctorCall+="');wrapper.setTool(tool)"; // pass the concrete instance in the C++ world
    p.Exec(ctorCall.c_str()); 
    p.Exec("ROOT.SetOwnership( tool , False )"); 
    // it is then possible to attach dynamically attributes to this instance (such as is done in py_XXX)
    p.Exec("tool.py_postInit()");      
    // call the user script :
    if(scriptName!="") p.LoadMacro(scriptName.c_str());
    // execute the user call :
    p.Exec(configCall.c_str());
    // initialize the top tool and it's subtools :
    p.Exec("tool.py_initialize()");

    std::cout << " ToolWrapper done for "<< m_tool << std::endl;
    return m_tool;
}
