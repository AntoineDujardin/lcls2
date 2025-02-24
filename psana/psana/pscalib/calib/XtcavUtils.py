"""
    Usage ::
    from psana.pscalib.calib.XtcavUtils import dict_from_xtcav_calib_object,\
                                               xtcav_calib_object_from_dict, Empty
"""

import logging
logger = logging.getLogger(__name__)

import numpy as np
from psana.pscalib.calib.XtcavConstants import Empty, Load, Save, ConstTest
from psana.pscalib.calib.MDBConvertUtils import serialize_dict

#--------------------

def dict_from_xtcav_calib_object(o):
    '''xtcav calibration object (a python object) has a list of attributes. 
    Each attribute may be simple or compaund type. 
    This method wraps all attributes except "__*" in the dictionary of pairs
    {<attr_name> : <attr_value>} and returns this dict.
    '''
    return dict([(name, getattr(o, name, None)) for name in dir(o) if name[:2] != '__'])

#--------------------

def xtcav_calib_object_from_dict(d):
    '''Converts input dictionary in xtcav calibration object and returns this object.
    Top level of dict (k,v) pairs is converted in the empty object attributes using setattr(o,str(k),v)    
    '''
    o = Empty()
    #assert isinstance(d, dict), 'Input parameter is not a python dict: %s' % str(d)
    if not isinstance(d, dict) :
        raise TypeError('Input parameter is not a python dict: %s' % str(d))
        #logging.warning('Input parameter is not a python dict: %s' % str(d))
        #return o
    for k,v in d.items() :
        setattr(o,str(k),v)
    return o

#------------------------------

def load_xtcav_calib_file(fname) :
    """Returns dict made of xtcav object loaded from hdf5 file by XtcavConstants.Load method.
    """
    logger.info('Load xtcav calib object from file: %s'%fname)
    o = Load(fname)
    return dict_from_xtcav_calib_object(o)

#------------------------------

def save_xtcav_calib_file(d, fname='ConstTest.h5') :
    """Saves dict -> xtcav calib object in hdf5 file.
    """
    logger.info('Save xtcav calib object in file: %s'%fname)
    o = xtcav_calib_object_from_dict(d)
    Save(o, fname)

#--------------------
#--------------------
#--------------------
#--------------------

if __name__ == "__main__":

  def test_const():
    ct = ConstTest()
    Save(ct,'ConstTest.h5')
    data = Load('ConstTest.h5')
    print('***',dir(data),data.parameters)

#--------------------

  def test_serialize_xtcav_dict():
      from psana.pscalib.calib.MDBConvertUtils import print_dict
      #fname = '/reg/d/psdm/AMO/amox23616/calib/'\
      #        'Xtcav::CalibV1/XrayTransportDiagnostic.0:Opal1000.0/lasingoffreference/56-end.data'
      #fname = '/reg/g/psdm/detector/data_test/calib/'\
      #        'Xtcav::CalibV1/XrayTransportDiagnostic.0:Opal1000.0/lasingoffreference/56-end.data'
      fname = '/reg/g/psdm/detector/data_test/calib/'\
              'Xtcav::CalibV1/XrayTransportDiagnostic.0:Opal1000.0/pedestals/104-end.data'
      o = Load(fname)
      dico = dict_from_xtcav_calib_object(o)
      serialize_dict(dico)
      #print('dico: %s' % dico)
      print(80*'_')
      print_dict(dico)
      print(80*'_')

#--------------------
        
if __name__ == "__main__":

    import sys
    logging.basicConfig(format='%(message)s', level=logging.DEBUG)

    tname = sys.argv[1] if len(sys.argv) > 1 else '1'
    logger.info('%s\nTest %s:' % (50*'_',tname))
    if   tname == '0' : test_const()
    elif tname == '1' : test_serialize_xtcav_dict()
    else : logger.info('Not-recognized test name: %s' % tname)
    sys.exit('End of test %s' % tname)

#--------------------
