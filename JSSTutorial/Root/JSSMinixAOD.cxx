#include <EventLoop/Worker.h>

// package include(s):
#include <JSSTutorial/JSSMinixAOD.h>

// this is needed to distribute the algorithm to the workers
ClassImp(JSSMinixAOD)


JSSMinixAOD :: JSSMinixAOD ()
{
  // Here you put any code for the base initialization of variables,
  // e.g. initialize all pointers to 0.  Note that you should only put
  // the most basic initialization here, since this method will be
  // called on both the submission and the worker node.  Most of your
  // initialization code will go into histInitialize() and
  // initialize().
  msg().setName( "JSSMinixAOD" );

  ATH_MSG_INFO( "Calling constructor");


}

EL::StatusCode JSSMinixAOD :: initialize ()
{

  MinixAOD::initialize();
  m_event = wk()->xaodEvent();
  
  for( std::string & varSpec : m_selectedAuxVars){
    size_t pos =varSpec.find_first_of('.');
    if (pos == std::string::npos ) {ATH_MSG_ERROR(" initialize() : can interpert variable specification : "<< varSpec);}
    m_event->setAuxItemList( varSpec.substr(0,pos+1), varSpec.substr(pos) ); // include the '.' in the first arg
  }
  
  ATH_MSG_INFO( "JSSMinixAOD Interface succesfully initialized!" );
  return EL::StatusCode::SUCCESS;
}
