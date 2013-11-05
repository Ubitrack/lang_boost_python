
__author__ = 'jack'

from nose import with_setup

from ubitrack.core import math as utmath
from ubitrack.core import util
from ubitrack.core import measurement
from ubitrack.facade import facade
import numpy as np
import time
import os

f = None

def setup_func():
    "set up test fixtures"
    util.initLogging("log4cpp.conf")
    global f
    f = facade.AdvancedFacade("/usr/local/lib/ubitrack")


def teardown_func():
    "tear down test fixtures"
    if f is not None:
        f.clearDataflow()


@with_setup(setup_func, teardown_func)
def test_basic_facade_components():

    thisdir = os.path.dirname(__file__)
    f.loadDataflow(os.path.join(thisdir, "single_pushsinkpose.dfg"), True)
    
    results = []

    def cb(m):
        results.append(m)
    
    x = f.getApplicationPushSinkPose("receiver")
    if x is None:
        raise RuntimeError("Wrapping is not working properly !!!!")
    
    x.setCallback(cb)

    f.setCallbackPose("receiver", cb)
    f.startDataflow()
    
    time.sleep(3)
    
    f.stopDataflow()

    assert len(results) > 0 


@with_setup(setup_func, teardown_func)
def test_pull_positionlist():

    thisdir = os.path.dirname(__file__)
    f.loadDataflow(os.path.join(thisdir, "test_positionlist.dfg"), True)
    
    x = f.getApplicationPullSinkPositionList("receiver")
    if x is None:
        raise RuntimeError("Wrapping is not working properly !!!!")
    
    f.startDataflow()
    
    mps = x.get(measurement.now())
    
    f.stopDataflow()

    ps = mps.get()
    assert len(ps) == 3 
    
    p0 = ps[0]
    assert p0[0] == 1 and p0[1] == 0 and p0[2] == 0
    

