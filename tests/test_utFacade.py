

from ubitrack.facade import facade
from ubitrack.core import util
import time

print "init logging"
util.initLogging("log4cpp.conf")

print "load dataflow"
f = facade.AdvancedFacade("/usr/local/lib/ubitrack")
f.loadDataflow("/home/jack/workspace/ubitrack_python/tests/single_pushsinkpose.dfg", True)


def cb(m):
    print "Measurement: %s" % m

print "get pushsink and register callback"
x = f.componentByName("receiver")
if x is None:
    raise RuntimeError("Wrapping is not working properly !!!!")

x.setCallback(cb)

#print "register callback again on the facade"
#f.setCallback("receiver", cb)

print "start dataflow"
f.startDataflow()

time.sleep(3)

print "stop dataflow"
f.stopDataflow()

print "cleanup"
f.clearDataflow()

print "done"
