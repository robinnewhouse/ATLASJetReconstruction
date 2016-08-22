import ROOT
from ROOT.asg import AsgTool
####
def generate_getter(prop_name,prop):
    """Generate a getter for the tool property. Doesn't work"""
    proptype = prop.__class__.__name__
    if prop.type()>0 and prop.type() < 6: # than it's a simple type
        def get_prop(self):
            pm = self.getPropertyMgr()
            prop = pm.getProperty(prop_name)
            return prop.pointer()[0]
    elif "ToolHandleArray" in proptype:
        def get_prop(self):
            try:
                return getattr(self,"_py"+prop_name)
            except:
                return []
    elif "ToolHandle" in proptype:
        def get_prop(self):
            try:
                return getattr(self,"_py"+prop_name)
            except:
                return None

    else:  # it must be a vector of simple type
        def get_prop(self):
            vec=self.getPropertyMgr().getProperty(prop_name).pointer()
            return list(vec)
    return get_prop

def generate_setter(prop_name,prop):
    """Generate a setter for the tool property"""
    proptype = prop.__class__.__name__
    #print "generate_setter ",prop_name , prop, prop.type(), proptype

    if prop_name == 'OutputLevel':
        # Need specialization for OutputLevel enum
        def set_prop(self, value):
            self.msg().setLevel(value)

    elif prop.type()>0 and prop.type() < 6: # then it's a simple type
        typ = {1:bool, 2:int, 3: float, 4:'double',5:str}[prop.type()]
        def set_prop(self, value):
            c = self.setProperty(typ)(prop_name, value)
            if c.isFailure():
                raise Exception('Error setting property: ' + prop_name)

    elif "ToolHandleArray" in proptype:
        handle=prop.pointer() # handle for a dummy tool
        interface = handle.__class__.__name__[16:-1] # extract XXX from "ToolHandleArray<XXX>"
        def set_prop(self, listOfTools):
            handle=self.getPropertyMgr().getProperty(prop_name).pointer() # handle for the actual tool
            for t in listOfTools:
                handle.push_back(ROOT.ToolHandle(interface)(t) )
            setattr(self,"_py"+prop_name, listOfTools) # save python pointers in the instance
            self._py_subTools.add( "_py"+prop_name )
            
    elif "ToolHandle" in proptype:
        handle=prop.pointer() # handle for a dummy tool
        interface = handle.__class__.__name__[11:-1] # extract XXX from "ToolHandle<XXX>"
        def set_prop(self, tool):
            handle=self.getPropertyMgr().getProperty(prop_name).pointer() # handle for the actual tool
            handle.__assign__(ROOT.ToolHandle(interface)(tool) )
            setattr(self,"_py"+prop_name, tool) # save python pointers in the instance
            self._py_subTools.add( "_py"+prop_name )
            
    else: # it must be a vector of simple type
        def set_prop(self, listofVal):
            vec=self.getPropertyMgr().getProperty(prop_name).pointer()
            vec.clear()
            for v in listofVal: vec.push_back(v)

    return set_prop


def pythonize_tool(toolClass):
    """Pythonize an ASG dual-use toolClass"""
    if hasattr(toolClass, '_isPythonized') : return
    toolClass._isPythonized = True
    dummy = toolClass("_dummy_")
    pm = dummy.getPropertyMgr()

    ## Prepare some function to extend the class 
    def init_with_prop(self,  name, **args ):
        """This redefines the __init__ so it accepts Properties as arguments like in
           MyTool('name', Prop1 = 3 ) (the c++ doesn't allow this).
           this will replace the existing __init__
        """
        self.__init_orig__(name)
        self.py_postInit()
        print "init_with_prop ",self
        for k,v in args.iteritems():
            setattr(self,k,v)
        
    def py_postInit(self):
        """to be called after original initialization to setup the python instance with additional helpers"""
        self._py_subTools = set([])

    def py_subTools(self):
        """returns all sub-tools owned by this tool, including self. Recursive."""
        tools = []
        for prop_name in self._py_subTools:
            toolList=getattr(self, prop_name)
            if isinstance(toolList,list):
                pass
            elif toolList is not None:
                toolList = [toolList]            
            for t in toolList:
                tools.extend( t.py_subTools() )
        tools.append(self)
        return tools
    
    def py_initialize(self):
        """Calls initialize on self and all its subtools. """
        for t in self.py_subTools():
            sc = t.initialize()
            if sc.isFailure() :
                print "ERROR in initialization of Top level tool ", t.name()
                print "ERROR   --> failed to initialize ",t.name(), " of type ",type(t)
                return sc
        return sc
            
    # add the new function to the class
    toolClass.__init_orig__ = toolClass.__init__
    toolClass.__init__ = init_with_prop
    toolClass.py_subTools = py_subTools
    toolClass.py_initialize = py_initialize
    toolClass.py_initialize = py_initialize
    toolClass.py_postInit = py_postInit

    # Loop over toolClass C++ properties and build a python property for each of them
    for prop_name, prop in pm.getProperties():
        prop = pm.getProperty(prop_name)
        # Build python property attribute and bind it to toolClass 
        get_prop = generate_getter(prop_name,prop)
        set_prop = generate_setter(prop_name,prop)
        setattr(toolClass, prop_name, property(get_prop, set_prop))


###################################################
        
@classmethod
def _new(cls, *l,**d):
    self = AsgTool.__new__orig__(cls,l,d)
    #print "new ",cls, self, self.__class__
    pythonize_tool(self.__class__)
    return self
    
AsgTool.__new__orig__ = AsgTool.__new__
AsgTool.__new__ = _new
###################################################


def buildPseudoJetGetter(name, **args):
    pass
