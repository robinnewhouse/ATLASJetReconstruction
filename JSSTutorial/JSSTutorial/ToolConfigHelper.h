// this file is -*- C++ -*-
#ifndef TOOLCONFIGHELPER_TOOLCONFIGHELPER_H
#define TOOLCONFIGHELPER_TOOLCONFIGHELPER_H


#include "TObject.h"
#include "JetInterface/IJetExecuteTool.h"

class ToolWrapper : public TObject {
 public:
 ToolWrapper() : m_tool(NULL){}
  
  IJetExecuteTool* init(const std::string & className, const std::string & toolName, const std::string & scriptName, const std::string & configCall );

  void setTool(IJetExecuteTool* t) {m_tool =t;}

  IJetExecuteTool * m_tool;
  
protected:
  
  ClassDef(ToolWrapper, 1);
};

#endif
