// for editors : this file is -*- C++ -*- 
#ifndef JSSWTOPTAGGERDNN_H_
#define JSSWTOPTAGGERDNN_H_

#include "BoostedJetTaggers/JSSTaggerBase.h"
#include "AsgTools/AsgTool.h"
#include "BoostedJetTaggers/lwtnn/LightweightNeuralNetwork.h"
#include "BoostedJetTaggers/lwtnn/parse_json.h"

#include <TSystem.h>
#include <TFile.h>
#include <TF1.h>

#include <map>
#include <iostream>
#include <fstream>
#include <vector>


class JSSWTopTaggerDNN :  public JSSTaggerBase {
  ASG_TOOL_CLASS0(JSSWTopTaggerDNN)

  public:
    //Default - so root can load based on a name
    JSSWTopTaggerDNN(const std::string& name);

    // Default - so we can clean up
    ~JSSWTopTaggerDNN();

    JSSWTopTaggerDNN& operator=(const JSSWTopTaggerDNN& rhs);

    // Run once at the start of the job to setup everything
    StatusCode initialize();

  // IJSSTagger interface
  virtual bool tag(const xAOD::Jet& jet, bool /*decorate*/) const {return result(jet)==0; };

    // Get the tagging result -- run for every jet!
    int result(const xAOD::Jet& jet) const;

    // Retrieve score for a given DNN type (top/W)
    double getScore(const xAOD::Jet& jet) const;

    // Write decoration
    void writeDecoration(const xAOD::Jet& jet) const;

    // Update the jet substructure variables for each jet to use in DNN
    std::map<std::string,double> getJetProperties(const xAOD::Jet& jet) const;

    StatusCode finalize();

  private:

    std::string m_name;
    std::string m_APP_NAME;
    std::string m_wkptFileName;

    // DNN tools
    std::unique_ptr<lwt::LightweightNeuralNetwork> m_lwnn;

    TFile* m_wkptFile;
    TF1* m_wkpt_DNN;
    std::map<std::string, double> m_DNN_inputValues;   // variables for DNN

    // booleans for W/Top tagging
    bool m_wtag;
    bool m_ztag;
    bool m_toptag;

    // result from DNN
    double m_DNNscore;

    // level representing variables to use
    unsigned int m_level;

    // file names and TMVA parameters
    std::map<std::string,std::string> m_DNN_weights;
    std::string m_wkpt;
    std::string m_DNNmethod;

    // protection for jets out of kinematic range
    float m_ptMin;
    float m_etaMax;

    // string for decorating jets with DNN output
    std::string m_DNNdecorationName;
};

#endif
