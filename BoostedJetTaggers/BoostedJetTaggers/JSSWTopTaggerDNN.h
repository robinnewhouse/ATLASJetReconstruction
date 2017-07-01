// for editors : this file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

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

    // IJetSelectorTools interface
    virtual Root::TAccept tag(const xAOD::Jet& jet) const;

    // Retrieve score for a given DNN type (top/W)
    double getScore(const xAOD::Jet& jet) const;

    // Write the decoration to the jet
    void decorateJet(const xAOD::Jet& jet, float mcutH, float mcutL, float scoreCut, float scoreValue) const;

    // Update the jet substructure variables for each jet to use in DNN
    std::map<std::string,double> getJetProperties(const xAOD::Jet& jet) const;
    StatusCode finalize();

  private:
    std::string m_name;
    std::string m_APP_NAME;

    // DNN tools
    std::unique_ptr<lwt::LightweightNeuralNetwork> m_lwnn;
    std::map<std::string, double> m_DNN_inputValues;   // variables for DNN

    // inclusive config file
    std::string m_configFile;
    std::string m_tagType;
    std::string m_kerasConfigFileName;
    std::string m_kerasConfigFilePath;
    std::string m_kerasConfigOutputName;

    // parameters to store specific cut values
    std::string m_strMassCutLow;
    std::string m_strMassCutHigh;
    std::string m_strScoreCut;

    // functions that are configurable for specific cut values
    TF1* m_funcMassCutLow;
    TF1* m_funcMassCutHigh;
    TF1* m_funcScoreCut;

    // string for decorating jets with DNN output
    std::string m_decorationName;
};
#endif