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

    enum Result {

      OutOfRangeHighPt = -3,
      OutOfRangeLowPt = -2,
      OutOfRangeEta = -1,
      InvalidJet = 0,

      AllPassed = 1,
      MassPassMVAPass = 1,

      MassPassMVAFail     = 2,
      LowMassFailMVAPass  = 4,
      LowMassFailMVAFail  = 8,
      HighMassFailMVAPass = 16,
      HighMassFailMVAFail = 32,

    };

    //Default - so root can load based on a name
    JSSWTopTaggerDNN(const std::string& name);

    // Default - so we can clean up
    ~JSSWTopTaggerDNN();
    JSSWTopTaggerDNN& operator=(const JSSWTopTaggerDNN& rhs);

    // Run once at the start of the job to setup everything
    StatusCode initialize();

    // IJSSTagger interface
    virtual bool tag(const xAOD::Jet& jet, bool decorate) const {
      return (result(jet,decorate)==AllPassed);
    };

    // Get the tagging result -- run for every jet!
    Result result(const xAOD::Jet& jet, bool decorate=true) const;

    // Retrieve score for a given DNN type (top/W)
    double getScore(const xAOD::Jet& jet) const;

    // Write the decoration to the jet
    void decorateJet(const xAOD::Jet& jet, float mcutH, float mcutL, float scoreCut, float scoreValue) const;

    // Update the jet substructure variables for each jet to use in DNN
    std::map<std::string,double> getJetProperties(const xAOD::Jet& jet) const;

    // Update the jet constituents for this jet to use in DNN
    std::map<std::string,double> getJetConstituents(const xAOD::Jet& jet) const;

    StatusCode finalize();

  private:
    std::string m_name;
    std::string m_APP_NAME;

    // DNN tools
    std::unique_ptr<lwt::LightweightNeuralNetwork> m_lwnn;
    std::map<std::string, double> m_DNN_inputValues;   // variables for DNN

    // the kinematic bounds for the jet - these are in MeV (not GeV!)
    float m_jetPtMin;
    float m_jetPtMax;
    float m_jetEtaMax;

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