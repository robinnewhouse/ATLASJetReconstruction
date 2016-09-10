#include "JSSTutorial/ToolConfigHelper.h"
#include "TPython.h"
#include <iostream>

IJetExecuteTool* ToolWrapper::init(const std::string & className, const std::string & toolName, const std::string & scriptName, const std::string & configCall ){

    TPython p;
    p.Bind(this, "wrapper");
    bool res= p.Exec("import ROOT");
    if( !res) return NULL;
    res = p.Exec("import JSSTutorial.RootCoreConfigInit");
    if( !res) return NULL;

    // Create the concrete instance on the python side
    std::string ctorCall = "tool=ROOT."+className;
    ctorCall+="('"; ctorCall+=toolName;   // ctor
    ctorCall+="');wrapper.setTool(tool)"; // pass the concrete instance in the C++ world
    res = p.Exec(ctorCall.c_str()); 
    if( !res) return NULL;
    p.Exec("ROOT.SetOwnership( tool , False )"); 
    // it is then possible to attach dynamically attributes to this instance (such as is done in py_XXX)
    p.Exec("tool.py_postInit()");      
    if( !res) return NULL;
    // call the user script :
    //if(scriptName!="") p.LoadMacro(scriptName.c_str());
    if(scriptName!="") { 
      std::string execScript="execfile(JSSTutorial.RootCoreConfigInit.scriptPath('"; execScript+=scriptName; execScript+="') )";
      res = p.Exec(execScript.c_str());
      if( !res) return NULL;
    }
    // execute the user call :
    res = p.Exec(configCall.c_str());
    if( !res) return NULL;

    // initialize the top tool and it's subtools :
    res = p.Exec("sc=tool.py_initialize()");
    if( !res) return NULL;
    res = p.Exec("if sc.isFailure(): raise Exception('py_initialize failed on '+str(tool))");
    if( !res) return NULL;

    std::cout << " ToolWrapper done for "<< m_tool << std::endl;
    return m_tool;
}
