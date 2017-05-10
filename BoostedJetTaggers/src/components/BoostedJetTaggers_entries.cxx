#include "GaudiKernel/DeclareFactoryEntries.h"
#include "BoostedJetTaggers/SubstructureTopTagger.h"
#include "BoostedJetTaggers/SubstructureTopTaggerHelpers.h"
#include "BoostedJetTaggers/HEPTopTagger.h"

#include "BoostedJetTaggers/SubstructureTopTagger.h"
#include "BoostedJetTaggers/SubstructureTopTaggerHelpers.h"
#include "BoostedJetTaggers/BoostedXbbTagger.h"
#include "BoostedJetTaggers/SmoothedTopTagger.h"
#include "BoostedJetTaggers/SmoothedWZTagger.h"
#include "BoostedJetTaggers/JetQGTagger.h"


DECLARE_TOOL_FACTORY(SubstructureTopTagger)
DECLARE_TOOL_FACTORY(SmoothedWZTagger)
DECLARE_TOOL_FACTORY(SmoothedTopTagger)
DECLARE_TOOL_FACTORY(BoostedXbbTagger)
DECLARE_NAMESPACE_TOOL_FACTORY(HTTxAOD, HEPTopTagger)
DECLARE_NAMESPACE_TOOL_FACTORY(STTHelpers, TopTagSmoothMassCut)
DECLARE_NAMESPACE_TOOL_FACTORY(STTHelpers, TopTagSmoothTau32Cut)
DECLARE_NAMESPACE_TOOL_FACTORY(CP, JetQGTagger)

DECLARE_FACTORY_ENTRIES(JetRec) {
  DECLARE_TOOL(SubstructureTopTagger)
  DECLARE_NAMESPACE_TOOL(HTTxAOD, HEPTopTagger)
  DECLARE_NAMESPACE_TOOL(STTHelpers, TopTagSmoothMassCut)
  DECLARE_NAMESPACE_TOOL(STTHelpers, TopTagSmoothTau32Cut)
  DECLARE_TOOL(SmoothedWZTagger)
  DECLARE_TOOL(SmoothedTopTagger)
  DECLARE_TOOL(BoostedXbbTagger)
  DECLARE_NAMESPACE_TOOL(CP, JetQGTagger)
  
}
