#include "BoostedJetTaggers/SubstructureTopTaggerHelpers.h"
#include "JetSelectorTools/JetAttributeSelector.h"
#include <limits>


#define CHECKPROP( code ) if( ( code ).isFailure() ) { std::cout<< "SubstructureTopTaggerHelpers  ERROR  can't call setProperty "<< std::endl;return NULL; } 

namespace STTHelpers {

  // Helper
  bool convertTofloat(std::string &s, float&v){
    try {
      v = std::stof(s);
    } catch (const std::invalid_argument& ia) { return false;}
    return true;
  }
  

  bool interpretCutString(const std::string &in, std::string &def, float &min, float &max);
  JetAttributeSelector * selectorFromString(const std::string &desc, const std::string &parentName);
  JetAttributeSelector * buildRatioSelector(const std::string& tname, const std::string& num, const std::string& denom  ) ;
  JetAttributeSelector * buildSelector(const std::string& tname, const std::string& att, bool absSelector=false  ) ;



  TopTagSmoothMassCut * defaultSmoothMassSelector(const std::string& targetEff, int etaRegion ){
    TopTagSmoothMassCut * sel = new TopTagSmoothMassCut("defaultSmoothMassCut"+targetEff);
    //std::cout << " defaultSmoothMassCut "<< targetEff << " eta="<< etaRegion<< std::endl;

    sel->setXValues( { 250000.000,325000.000,375000.000,425000.000,475000.000,525000.000,575000.000,625000.000,675000.000,725000.000,775000.000,850000.000,950000.000,1100000.000,1300000.000,1680000.000 } );

    if(targetEff == "50"){
      if(etaRegion==0) sel->setYValues( {73218.019,86495.589,95347.302,102628.033,108026.246,111216.308,113236.508,113933.951,114572.100,114794.770,115262.890,115881.850,117088.318,119025.750,122143.515,128067.270} );
      else if (etaRegion==1) sel->setYValues( {83492.838,96844.753,105746.029,113281.475,118856.066,122190.000,123690.000,124130.410,123863.458,123371.891,123325.597,123531.494,124076.533,125102.721,126372.422,128784.854} );
     else if (etaRegion==2) sel->setYValues( {85052.983,98705.422,107807.048,115186.721,120365.410,123510.000,125010.000,125662.377,126075.960,126389.113,126537.840,126803.137,127322.903,128379.386,130241.032,133778.159} );       


    } else if (targetEff == "80"){
      if(etaRegion==0) sel->setYValues( { 68424.912,72205.507,74725.904,76559.341,78043.033,78870.000,79110.000,79344.836,79644.836,79897.763,80434.288,81870.018,85076.854,91634.790,102461.248,123031.518} );
      else if (etaRegion==1) sel->setYValues( { 71268.258,74276.825,76282.537,77498.195,78338.361,78390.000,77910.000,77658.197,78306.004,79810.687,82069.306,86364.812,92190.427,99833.265,108647.398,125394.249  } );
      else if (etaRegion==2) sel->setYValues( {67888.967,72014.026,74764.066,76769.667,78354.344,79170.000,79530.000,80158.525,81195.851,82779.245,84890.965,88747.162,94262.629,102710.787,113868.253,135067.438  });
    }

    return sel;
  }

  TopTagSmoothTau32Cut * defaultSmoothTau32Selector(const std::string& targetEff, int etaRegion){
    TopTagSmoothTau32Cut * sel = new TopTagSmoothTau32Cut("defaultSmoothTau32Cut"+targetEff);
    sel->setXValues( { 250000.000,325000.000,375000.000,425000.000,475000.000,525000.000,575000.000,625000.000,675000.000,725000.000,775000.000,850000.000,950000.000,1100000.000,1300000.000,1680000.000 } );
    if(targetEff == "50"){
      if(etaRegion==0) sel->setYValues( { 0.675,0.634,0.607,0.585,0.568,0.557,0.552,0.550,0.551,0.553,0.556,0.560,0.563,0.566,0.568,0.570 });
      else if (etaRegion==1) sel->setYValues( {0.755,0.701,0.665,0.634,0.612,0.599,0.592,0.590,0.591,0.594,0.597,0.602,0.607,0.612,0.616,0.622 } );
      else if (etaRegion==2) sel->setYValues( {0.773,0.713,0.672,0.637,0.610,0.591,0.579,0.574,0.573,0.574,0.576,0.578,0.580,0.580,0.577,0.571} );


    } else if (targetEff == "80") {      
      if(etaRegion==0) sel->setYValues( { 0.832,0.791,0.763,0.739,0.719,0.705,0.694,0.688,0.683,0.681,0.680,0.680,0.681,0.682,0.684,0.688} );
      else if (etaRegion==1) sel->setYValues( { 0.880,0.839,0.811,0.785,0.763,0.746,0.734,0.727,0.723,0.722,0.722,0.725,0.728,0.730,0.730,0.730 });
      else if (etaRegion==2) sel->setYValues( { 0.879,0.831,0.799,0.770,0.746,0.727,0.714,0.706,0.701,0.698,0.698,0.699,0.700,0.701,0.699,0.696  });

    }
    return sel;
  }





  ///**************************************************
  SubstructureTopTagger * configSubstTagger(const std::string &tagname, const std::vector<std::string> & cutdescs){

    SubstructureTopTagger * sttTool = new SubstructureTopTagger(tagname);
    CHECKPROP( sttTool->setProperty("TaggerName", tagname) ) ;

    ToolHandleArray<IJetSelector> selArray;
    for( const std::string & desc : cutdescs) {
      //std::cout << " configSubstTagger : "<< desc << std::endl;
      IJetSelector * selector =NULL ;
      if     (desc == "SmoothMassCut_50_loweta") selector = defaultSmoothMassSelector("50",0);
      else if(desc == "SmoothMassCut_80_loweta") selector = defaultSmoothMassSelector("80",0);
      else if(desc == "SmoothMassCut_50_higheta") selector = defaultSmoothMassSelector("50",1);
      else if(desc == "SmoothMassCut_80_higheta") selector = defaultSmoothMassSelector("80",1);
      else if(desc == "SmoothTau32Cut_50_loweta") selector = defaultSmoothTau32Selector("50",0);
      else if(desc == "SmoothTau32Cut_80_loweta") selector = defaultSmoothTau32Selector("80",0);
      else if(desc == "SmoothTau32Cut_50_higheta") selector = defaultSmoothTau32Selector("50",1);
      else if(desc == "SmoothTau32Cut_80_higheta") selector = defaultSmoothTau32Selector("80",1);

      else if(desc == "SmoothMassCut_50") selector = defaultSmoothMassSelector("50",2);
      else if(desc == "SmoothMassCut_80") selector = defaultSmoothMassSelector("80",2);
      else if(desc == "SmoothTau32Cut_50") selector = defaultSmoothTau32Selector("50",2);
      else if(desc == "SmoothTau32Cut_80") selector = defaultSmoothTau32Selector("80",2);
      else { // interpret as "v1< attribute < v2"
        selector = selectorFromString( desc , tagname);
      }
      if( ! selector ) {
        std::cout << "STTHelpers::configSubstTagger  ERROR  can't create tool from "<< desc<< std::endl;
        return NULL;
      }
      if ( ! selector->initialize() ){
        std::cout << "STTHelpers::configSubstTagger  ERROR  can't initialize tool "<< selector->name()<< " from "<< desc<< std::endl;
        return NULL;
      }
      selArray.push_back( ToolHandle<IJetSelector>( selector ) );
    } //  

    CHECKPROP( sttTool->setProperty("SelectionTools", selArray) );
    return sttTool;
  }

  SubstructureTopTagger * configSubstTagger(const std::string &tagname, const std::vector<std::vector<std::string> > & cutdescsVec){
    if (cutdescsVec.empty() ) return NULL;
    if (cutdescsVec.size()==1 ) return configSubstTagger( tagname, cutdescsVec[0] );

    SubstructureTopTagger * sttTool = new SubstructureTopTagger(tagname);
    CHECKPROP( sttTool->setProperty("TaggerName", tagname) );
    ToolHandleArray<IJetSelector> selArray;
    for( const std::vector<std::string>& cutdescs : cutdescsVec) {
      SubstructureTopTagger * t = configSubstTagger( tagname, cutdescs);
      if( t==NULL) return NULL;
      t->initialize();
      selArray.push_back( ToolHandle<IJetSelector>( t ) );      
    }
    CHECKPROP( sttTool->setProperty("CombineWithAND", false) );
    CHECKPROP( sttTool->setProperty("SelectionTools", selArray) );
    return sttTool;
  }


  SubstructureTopTagger * configSubstTagger(const std::string &tagname, const std::string & taggingShortCut){
    if  (taggingShortCut == "SmoothCut_50") {
      return configSubstTagger(tagname, std::vector<std::string>{ "SmoothMassCut_50", "SmoothTau32Cut_50"  });      
    }
    else if (taggingShortCut == "SmoothCut_80") {
      return configSubstTagger(tagname, std::vector<std::string>{ "SmoothMassCut_80", "SmoothTau32Cut_80"  });
    }
    
    else if (taggingShortCut == "FixedCut_LowPt_50") {
      return configSubstTagger(tagname, std::vector<std::string>{ "107000<m" , "Tau32_wta<0.67" } );
    }                                   
    else if (taggingShortCut == "FixedCut_LowPt_80") {
      return configSubstTagger(tagname, std::vector<std::string>{ "74000<m" , "Tau32_wta<0.78" } );
      
    }                                   
    else if (taggingShortCut == "FixedCut_HighPt_50") {
      return configSubstTagger(tagname, std::vector<std::string>{ "125000<m" , "Tau32_wta<0.57" } );
    }                                   
    else if (taggingShortCut == "FixedCut_HighPt_80") {
      return configSubstTagger(tagname, std::vector<std::string>{ "80000<m" , "Tau32_wta<0.71" } );      
    }                                   
    else {
      std::cout << " SubstructureTopTagger::configSubstTagger ERROR : unkown tagging shortcut : "<< taggingShortCut << std::endl;
    }
    return NULL;
    
  }



  ///**************************************************

  JetAttributeSelector * selectorFromString(const std::string &desc, const std::string &parentName){
    std::string cut;
    float min = -std::numeric_limits<float>::max();
    float max =  std::numeric_limits<float>::max();
    bool ok = interpretCutString(desc, cut, min,max);
    if(!ok) {
      std::cout<< "SubstructureTopTaggerHelpers::selectorFromString ERROR : can't create JetAttributeSelector from '"<< desc <<"'"<< std::endl;
      return NULL;
    }
    
    JetAttributeSelector * attsel=NULL;
    std::string cutToolName = parentName+cut;
    if( cut == "Tau32_wta") attsel = buildRatioSelector(cutToolName, "Tau3_wta", "Tau2_wta");
    else if( cut == "Tau21_wta") attsel =  buildRatioSelector(cutToolName, "Tau2_wta", "Tau1_wta");
    else if( cut == "abs(eta)" ) attsel = buildSelector(cutToolName, "eta", true);
    else attsel = buildSelector(cutToolName, cut);
    CHECKPROP( attsel->setProperty("CutMin",min) );
    CHECKPROP( attsel->setProperty("CutMax",max) );

    return attsel;  
  }




  JetAttributeSelector * buildRatioSelector(const std::string& tname, const std::string& num, const std::string& denom  ) {
    JetAttributeRatioSelector* sel  = new JetAttributeRatioSelector(tname);
    //std::cout << " building JetAttributeRatioSelector  "<< tname << "  attribute ="<< num << "   "<< denom <<std::endl;
    CHECKPROP( sel->setProperty("Attribute", num) );
    CHECKPROP( sel->setProperty("Attribute2", denom) );
    return sel;
  }

  JetAttributeSelector * buildSelector(const std::string& tname, const std::string& att, bool absSelector  ) {
    JetAttributeSelector* sel;
    if(absSelector) sel= new JetAbsAttributeSelector(tname);
    else sel= new JetAttributeSelector(tname);
    //std::cout << " building JetAttributeSelector  "<< tname << "  attribute ="<< att << std::endl;
    CHECKPROP( sel->setProperty("Attribute", att) );
    return sel;
  }

  
  bool interpretCutString(const std::string &in, std::string &def, float &min, float &max){
    min = -std::numeric_limits<float>::max();
    max = std::numeric_limits<float>::max();

    std::stringstream str(in);
    std::string s1,s2,s3;
    std::getline(str, s1, '<' );
    std::getline(str, s2, '<' );
    std::getline(str, s3, '<' );

    bool converted;
    if(s3==""){
      converted =convertTofloat(s1, min);
      if( converted) def = s2;
      else {
        converted = convertTofloat(s2,max);
        if(converted) def = s1;
      }
      if(!converted) {
        std::cout<< "TopTagging Config ERROR :  Can't interpret "<< s1 << " nor "<< s2<< "  from cut description "<< in <<std::endl;
      } 
    }else{
      def = s2;
      converted = convertTofloat(s1, min);
      converted = converted && convertTofloat(s3, max);
      if(!converted) {
        std::cout<< "TopTagging Config ERROR : Can't interpret "<< s2 << " or "<< s3<< "  from cut description "<< in << std::endl;
      }
    } 
  
    // remove all whitespace
    def.erase (std::remove (def.begin(), def.end(), ' '), def.end());

    return converted;
  } 






  ///**************************************************
  ///**************************************************
  /// TopTagging IJetSelector implementations
  ///**************************************************

  void Interpolator::setValues(const std::vector<float>& x, const std::vector<float>& y){
    m_xvalues=x; m_yvalues=y;
    initValues();
  }

  void Interpolator::setXValues(const std::vector<float>& x){
    m_xvalues=x;     initValues();
  }
  

  void Interpolator::setYValues(const std::vector<float>& y){
    m_yvalues=y;     initValues();
  }

  void Interpolator::initValues(){
    if(! m_xvalues.empty()) {
      m_xmin = m_xvalues.front();
      m_xmax = m_xvalues.back();      
    }
    if(! m_yvalues.empty()) {
      m_ymin = m_yvalues.front();
      m_ymax = m_yvalues.back();      
    }
  }
  
  float Interpolator::eval(float x) const {
    if(x<m_xmin) return m_ymin;
    if(x>m_xmax) return m_ymax;
    int i=0; 
    float xlow, xup;
    while(m_xvalues[i+1]<x) i++; 
    xlow = m_xvalues[i]; xup = m_xvalues[i+1];    
    float a = (m_yvalues[i+1] - m_yvalues[i])/( xup - xlow );
    float b = m_yvalues[i] - a*xlow;
    return a*x+b;
  }




  ///**************************************************
  TopTagSmoothMassCut::TopTagSmoothMassCut(const std::string& name):  asg::AsgTool(name){
    declareProperty("PtValues",m_xvalues);
    declareProperty("MassValues",m_yvalues);
  }

  int TopTagSmoothMassCut::keep(const xAOD::Jet& jet) const {
    int r = keep(jet.pt(), jet.m() );
    return r ;
  }

  int TopTagSmoothMassCut::keep(double cal_pt, double cal_m) const {
    double mcut = eval(cal_pt);
    //std::cout << " TopTagSmoothMassCut :: keep "<< cal_pt << "  "<< mcut << "  "<<cal_m<< std::endl;
    return (mcut<cal_m);
  }



  ///**************************************************
  TopTagSmoothTau32Cut::TopTagSmoothTau32Cut(const std::string& name):  asg::AsgTool(name){
    declareProperty("PtValues",m_xvalues);
    declareProperty("Tau32Values",m_yvalues);
  }


  int TopTagSmoothTau32Cut::keep(const xAOD::Jet& jet) const {
    static SG::AuxElement::Accessor<float> tau3Acc("Tau3_wta");
    static SG::AuxElement::Accessor<float> tau2Acc("Tau2_wta");    
    static SG::AuxElement::Accessor<float> tau32Acc("Tau32_wta");    
    if( tau32Acc.isAvailable( jet ) ) {
      return keep( jet.pt(), tau32Acc(jet ) ) ;
    }else {
      return keep(jet.pt(), tau3Acc(jet), tau2Acc(jet));
    }
  }

  int TopTagSmoothTau32Cut::keep( double pt, double tau32) const {
    return (tau32 < eval(pt) );    
  }

  int TopTagSmoothTau32Cut::keep( double pt, double tau3, double tau2) const {
    if(tau2==0) return 0;
    double tau32 = tau3/ tau2;
    return keep(pt, tau32);
  }
  

  void __test(){
    SubstructureTopTagger * t = configSubstTagger("MassAndTau32Tag", 
                                                  {   {"-1<eta<1","120000<m", "Tau32_wta<0.61"} ,
                                                      {"-1<eta<1","120000<m", "Tau32_wta<0.61"} } );
    std::cout << t->name() << std::endl;
  }
}
