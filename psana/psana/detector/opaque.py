import hsd as Hsd

def version2str(version):
    return str(version[0]) + '.' + str(version[1]) + '.' + str(version[2])

def OpaqueRawData(name, config):
    sw = getattr(config, 'software')
    det = getattr(sw, name)
    df = eval(str(det.dettype) + '._Factory()')
    return df.create(name, config)

class OpaqueRawDataBase(object):
    def __init__(self, name, config):
        self.detectorName = name
        self.config = config

class hsd(OpaqueRawDataBase):
    """
    hsd reader
    """
    def __init__(self, name, config):
        super(hsd, self).__init__(name, config)
        self.name = name
        sw = getattr(config, 'software')
        detcfg = getattr(sw, name)
        self.nchan = getattr(config, 'nchan')
        assert detcfg.dettype == 'hsd'
        assert detcfg.hsd.software == 'hsd'
        self.version = detcfg.hsd.version

    def __call__(self, evt):
        chans = []
        for i in range(self.nchan):
            chans.append( eval('evt._dgrams[0].xpphsd.hsd.chan'+str(i)) ) # FIXME: find out 0 in dgrams[0]
        nonOpaqueHsd = Hsd.hsd(version2str(self.version), chans)  # make an object per event, calls hsd.pyx def hsd()
        return nonOpaqueHsd

    class _Factory:
        def create(self, name, config): return hsd(name, config)
