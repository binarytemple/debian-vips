# This file was automatically generated by SWIG (http://www.swig.org).
# Version 1.3.36
#
# Don't modify this file, modify the SWIG interface instead.
# This file is compatible with both classic and new-style classes.

import vmaskmodule
import new
new_instancemethod = new.instancemethod
try:
    _swig_property = property
except NameError:
    pass # Python < 2.2 doesn't have 'property'.
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'PySwigObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

def _swig_repr(self):
    try: strthis = "proxy of " + self.this.__repr__()
    except: strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0
del types


import VError
import VImage
import VDisplay
class VMask(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, VMask, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, VMask, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = vmaskmodule.new_VMask(*args)
        try: self.this.append(this)
        except: self.this = this
    def __assign__(*args): return vmaskmodule.VMask___assign__(*args)
    __swig_destroy__ = vmaskmodule.delete_VMask
    __del__ = lambda self : None;
    def xsize(*args): return vmaskmodule.VMask_xsize(*args)
    def ysize(*args): return vmaskmodule.VMask_ysize(*args)
    def size(*args): return vmaskmodule.VMask_size(*args)
    def filename(*args): return vmaskmodule.VMask_filename(*args)
    def type(*args): return vmaskmodule.VMask_type(*args)
    def mask(*args): return vmaskmodule.VMask_mask(*args)
    def ostream_print(*args): return vmaskmodule.VMask_ostream_print(*args)
VMask_swigregister = vmaskmodule.VMask_swigregister
VMask_swigregister(VMask)

class VIMask(VMask):
    __swig_setmethods__ = {}
    for _s in [VMask]: __swig_setmethods__.update(getattr(_s,'__swig_setmethods__',{}))
    __setattr__ = lambda self, name, value: _swig_setattr(self, VIMask, name, value)
    __swig_getmethods__ = {}
    for _s in [VMask]: __swig_getmethods__.update(getattr(_s,'__swig_getmethods__',{}))
    __getattr__ = lambda self, name: _swig_getattr(self, VIMask, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = vmaskmodule.new_VIMask(*args)
        try: self.this.append(this)
        except: self.this = this
    def scale(*args): return vmaskmodule.VIMask_scale(*args)
    def offset(*args): return vmaskmodule.VIMask_offset(*args)
    def embed(*args): return vmaskmodule.VIMask_embed(*args)
    def __index__(*args): return vmaskmodule.VIMask___index__(*args)
    def __call__(*args): return vmaskmodule.VIMask___call__(*args)
    def get(*args): return vmaskmodule.VIMask_get(*args)
    __swig_getmethods__["gauss"] = lambda x: vmaskmodule.VIMask_gauss
    if _newclass:gauss = staticmethod(vmaskmodule.VIMask_gauss)
    __swig_getmethods__["gauss_sep"] = lambda x: vmaskmodule.VIMask_gauss_sep
    if _newclass:gauss_sep = staticmethod(vmaskmodule.VIMask_gauss_sep)
    __swig_getmethods__["log"] = lambda x: vmaskmodule.VIMask_log
    if _newclass:log = staticmethod(vmaskmodule.VIMask_log)
    def rotate45(*args): return vmaskmodule.VIMask_rotate45(*args)
    def rotate90(*args): return vmaskmodule.VIMask_rotate90(*args)
    def trn(*args): return vmaskmodule.VIMask_trn(*args)
    def inv(*args): return vmaskmodule.VIMask_inv(*args)
    def cat(*args): return vmaskmodule.VIMask_cat(*args)
    def mul(*args): return vmaskmodule.VIMask_mul(*args)
    __swig_destroy__ = vmaskmodule.delete_VIMask
    __del__ = lambda self : None;
VIMask_swigregister = vmaskmodule.VIMask_swigregister
VIMask_swigregister(VIMask)
__lshift__ = vmaskmodule.__lshift__
VIMask_gauss = vmaskmodule.VIMask_gauss
VIMask_gauss_sep = vmaskmodule.VIMask_gauss_sep
VIMask_log = vmaskmodule.VIMask_log

class VDMask(VMask):
    __swig_setmethods__ = {}
    for _s in [VMask]: __swig_setmethods__.update(getattr(_s,'__swig_setmethods__',{}))
    __setattr__ = lambda self, name, value: _swig_setattr(self, VDMask, name, value)
    __swig_getmethods__ = {}
    for _s in [VMask]: __swig_getmethods__.update(getattr(_s,'__swig_getmethods__',{}))
    __getattr__ = lambda self, name: _swig_getattr(self, VDMask, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = vmaskmodule.new_VDMask(*args)
        try: self.this.append(this)
        except: self.this = this
    def embed(*args): return vmaskmodule.VDMask_embed(*args)
    def scale(*args): return vmaskmodule.VDMask_scale(*args)
    def offset(*args): return vmaskmodule.VDMask_offset(*args)
    def __index__(*args): return vmaskmodule.VDMask___index__(*args)
    def __call__(*args): return vmaskmodule.VDMask___call__(*args)
    def get(*args): return vmaskmodule.VDMask_get(*args)
    __swig_getmethods__["gauss"] = lambda x: vmaskmodule.VDMask_gauss
    if _newclass:gauss = staticmethod(vmaskmodule.VDMask_gauss)
    __swig_getmethods__["log"] = lambda x: vmaskmodule.VDMask_log
    if _newclass:log = staticmethod(vmaskmodule.VDMask_log)
    def rotate45(*args): return vmaskmodule.VDMask_rotate45(*args)
    def rotate90(*args): return vmaskmodule.VDMask_rotate90(*args)
    def scalei(*args): return vmaskmodule.VDMask_scalei(*args)
    def trn(*args): return vmaskmodule.VDMask_trn(*args)
    def inv(*args): return vmaskmodule.VDMask_inv(*args)
    def cat(*args): return vmaskmodule.VDMask_cat(*args)
    def mul(*args): return vmaskmodule.VDMask_mul(*args)
    __swig_destroy__ = vmaskmodule.delete_VDMask
    __del__ = lambda self : None;
VDMask_swigregister = vmaskmodule.VDMask_swigregister
VDMask_swigregister(VDMask)
VDMask_gauss = vmaskmodule.VDMask_gauss
VDMask_log = vmaskmodule.VDMask_log



