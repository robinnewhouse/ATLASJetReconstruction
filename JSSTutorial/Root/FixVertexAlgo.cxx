#include <EventLoop/Job.h>
#include <EventLoop/Worker.h>

#include "JSSTutorial/FixVertexAlgo.h"
#include <iostream>
#include "xAODTracking/VertexContainer.h"

#include "AsgTools/MsgStream.h" // needed for ATH_MSG_XXX

#include <xAODAnaHelpers/HelperFunctions.h>
#include <xAODAnaHelpers/tools/ReturnCheck.h>


// this is needed to distribute the algorithm to the workers
ClassImp(FixVertexAlgo)


FixVertexAlgo :: FixVertexAlgo  () : m_optionString("{}")
{
  // Here you put any code for the base initialization of variables,
  // e.g. initialize all pointers to 0.  Note that you should only put
  // the most basic initialization here, since this method will be
  // called on both the submission and the worker node.  Most of your
  // initialization code will go into histInitialize() and
  // initialize().

  msg().setName( "FixVertexAlgo" );

  ATH_MSG_INFO( "Calling constructor");

}

EL::StatusCode FixVertexAlgo :: setupJob (EL::Job& job)
{
  // Here you put code that sets up the job on the submission object
  // so that it is ready to work with your algorithm, e.g. you can
  // request the D3PDReader service or add output files.  Any code you
  // put here could instead also go into the submission script.  The
  // sole advantage of putting it here is that it gets automatically
  // activated/deactivated when you add/remove the algorithm from your
  // job, which may or may not be of value to you.

  job.useXAOD();
  xAOD::Init( "FixVertexAlgo" ).ignore(); // call before opening first file

  // EL::OutputStream outForTree("tree");
  // job.outputAdd (outForTree);

  return EL::StatusCode::SUCCESS;

}

EL::StatusCode FixVertexAlgo :: histInitialize ()
{
  // Here you do everything that needs to be done at the very
  // beginning on each worker node, e.g. create histograms and output
  // trees.  This method gets called before any input files are
  // connected.
  return EL::StatusCode::SUCCESS;

}

EL::StatusCode FixVertexAlgo :: fileExecute ()
{
  // Here you do everything that needs to be done exactly once for every
  // single file, e.g. collect a list of all lumi-blocks processed


  return EL::StatusCode::SUCCESS;
}

EL::StatusCode FixVertexAlgo :: changeInput (bool /*firstFile*/)
{
  // Here you do everything you need to do when we change input files,
  // e.g. resetting branch addresses on trees.  If you are using
  // D3PDReader or a similar service this method is not needed.

  return EL::StatusCode::SUCCESS;

}

EL::StatusCode FixVertexAlgo :: initialize ()
{
  // Here you do everything that you need to do after the first input
  // file has been connected and before the first event is processed,
  // e.g. create additional histograms based on which variables are
  // available in the input files.  You can also create all of your
  // histograms and trees in here, but be aware that this method
  // doesn't get called if no events are processed.  So any objects
  // you create here won't be available in the outputif you have no
  // input events.

  m_event = wk()->xaodEvent();
  m_store = wk()->xaodStore();
  
  if(m_debug) msg().setLevel(MSG::DEBUG);
  return EL::StatusCode::SUCCESS;
}


EL::StatusCode FixVertexAlgo :: execute ()
{
  // Here you do everything that needs to be done on every single
  // events, e.g. read input variables, apply cuts, and fill
  // histograms and trees.  This is where most of your actual analysis
  // code will go.

  //if ( m_debug ) { Info("execute()", "Executing FixVertexAlgo..."); }
  ATH_MSG_DEBUG( "Executing " );
  const xAOD::VertexContainer* vertexContainer=0;
  RETURN_CHECK("FixVertexAlgo::execute()", HelperFunctions::retrieve(vertexContainer, "PrimaryVertices", m_event, m_store,  m_verbose), "");

  SG::AuxElement::Decorator<float> x("x");
  SG::AuxElement::Decorator<float> y("y");

  for(const xAOD::Vertex * v: *vertexContainer){
    x(*v) = 0.;
    y(*v) = 0.;
  }
  
  return EL::StatusCode::SUCCESS;

}

EL::StatusCode FixVertexAlgo :: postExecute ()
{
  // Here you do everything that needs to be done after the main event
  // processing.  This is typically very rare, particularly in user
  // code.  It is mainly used in implementing the NTupleSvc.

  return EL::StatusCode::SUCCESS;

}

EL::StatusCode FixVertexAlgo :: finalize ()
{
  // This method is the mirror image of initialize(), meaning it gets
  // called after the last event has been processed on the worker node
  // and allows you to finish up any objects you created in
  // initialize() before they are written to disk.  This is actually
  // fairly rare, since this happens separately for each worker node.
  // Most of the time you want to do your post-processing on the
  // submission node after all your histogram outputs have been
  // merged.  This is different from histFinalize() in that it only
  // gets called on worker nodes that processed input events.

  Info("finalize()", "Deleting tool instances...");

  return EL::StatusCode::SUCCESS;

}

EL::StatusCode FixVertexAlgo :: histFinalize ()
{
  // This method is the mirror image of histInitialize(), meaning it
  // gets called after the last event has been processed on the worker
  // node and allows you to finish up any objects you created in
  // histInitialize() before they are written to disk.  This is
  // actually fairly rare, since this happens separately for each
  // worker node.  Most of the time you want to do your
  // post-processing on the submission node after all your histogram
  // outputs have been merged.  This is different from finalize() in
  // that it gets called on all worker nodes regardless of whether
  // they processed input events.

  return EL::StatusCode::SUCCESS;

}
