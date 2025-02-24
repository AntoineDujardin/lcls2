#------------------------------
"""
Class :py:class:`CGConfigParameters` supports configuration parameters for application
======================================================================================

This software was developed for the SIT project.
If you use all or part of it, please give an appropriate acknowledgment.

Usage ::

    cp.readParametersFromFile()
    cp.printParameters()
    cp.log_level.setValue('DEBUG')
    cp.saveParametersInFile()

See:
    - :class:`CGWMain`
    - :class:`CGWMainControl`
    - `graphqt documentation <https://github.com/slac-lcls/lcls2/tree/master/psdaq/psdaq/control_gui>`_.

Created on 2019-06-10 by Mikhail Dubrovin
"""
#------------------------------

import logging
logger = logging.getLogger(__name__)

#------------------------------

#from psana.pyalgos.generic.ConfigParameters import ConfigParameters
from psdaq.control_gui.ConfigParameters import ConfigParameters

#------------------------------

class CGConfigParameters(ConfigParameters) :
    """A storage of configuration parameters for LCLS-2 DAQ Control GUI (CG)
    """
    char_expand    = u' \u25BC' # down-head triangle
    char_shrink    = u' \u25B2' # solid up-head triangle

    def __init__(self, fname=None) :
        """fname : str - the file name with configuration parameters, if not specified then use default.
        """
        ConfigParameters.__init__(self)

        logger.debug('In %s c-tor')

        #self.fname_cp = '%s/%s' % (os.path.expanduser('~'), '.cg-confpars.txt') # Default config file name
        self.fname_cp = './cg-confpars.txt' # Default config file name

        self.declareParameters()
        self.readParametersFromFile()
        #self.printParameters()

        # Registration of widgets/objects
        self.cgwmain     = None

#------------------------------
        
    def declareParameters(self) :

        # Possible typs for declaration : 'str', 'int', 'long', 'float', 'bool'
        self.log_level = self.declareParameter(name='LOG_LEVEL', val_def='INFO', type='str') # val_def='NOTSET'

        #self.log_file  = self.declareParameter(name='LOG_FILE_NAME', val_def='cm-log.txt', type='str')
        #self.log_prefix = self.declareParameter(name='LOG_FILE_PREFIX', val_def='/reg/g/psdm/logs/calibman/lcls2', type='str')
        #self.log_prefix = self.declareParameter(name='LOG_FILE_PREFIX', val_def='./cm-log', type='str')
        #self.save_log_at_exit = self.declareParameter(name='SAVE_LOG_AT_EXIT', val_def=True,  type='bool')
        #self.dir_log_global  = self.declareParameter(name='DIR_LOG_FILE_GLOBAL', val_def='/reg/g/psdm/logs/calibman', type='str')

        self.main_win_pos_x  = self.declareParameter(name='MAIN_WIN_POS_X',  val_def=100, type='int')
        self.main_win_pos_y  = self.declareParameter(name='MAIN_WIN_POS_Y',  val_def=5,   type='int')
        self.main_win_width  = self.declareParameter(name='MAIN_WIN_WIDTH',  val_def=370, type='int')
        self.main_win_height = self.declareParameter(name='MAIN_WIN_HEIGHT', val_def=810, type='int')

        #self.main_vsplitter  = self.declareParameter(name='MAIN_VSPLITTER', val_def=600, type='int')
        #self.main_tab_name   = self.declareParameter(name='MAIN_TAB_NAME', val_def='CDB', type='str')

#------------------------------
        
#    def __del__(self) :
#        logger.debug('In d-tor: %s')
#        logger.debug('In CGConfigParameters d-tor')
#        if self.save_cp_at_exit.value() :
#            self.saveParametersInFile()
#        #ConfigParameters.__del__(self)

#------------------------------

cp = CGConfigParameters()

#import psana.pscalib.calib.CalibConstants as cc
#cp.user = None #cc.USERNAME
#cp.upwd = ''   #cc.USERPW
#print('XXXX CGConfigParameters set cp.user: %s p: %s' % (cp.user, cp.upwd))

#------------------------------

def test_CGConfigParameters() :

    logging.basicConfig(format='%(message)s', level=logging.DEBUG)

    cp.readParametersFromFile()
    cp.printParameters()
    cp.log_level.setValue('DEBUG')
    cp.saveParametersInFile()

#------------------------------

if __name__ == "__main__" :
    import sys
    test_CGConfigParameters()
    sys.exit(0)

#------------------------------
