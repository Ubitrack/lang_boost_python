__author__ = 'jack'

from nose import with_setup

import ubitrack_python as ut
import pyublas # load converters
import numpy as np



def setup_func():
    "set up test fixtures"
    pass

def teardown_func():
    "tear down test fixtures"
    pass

@with_setup(setup_func, teardown_func)
def test_basic_datatypes():
    "test basic data types: Vector2-8, Matrix33-44, Pose, Quaternion"

    q = ut.test_quat()
    assert q.x() == 0 and q.y() == 0 and q.z() == 0 and q.w() == 1

    m = ut.test_mat33()
    assert np.all(m == np.identity(3))

    # from vector
    p = ut.Pose(q, m[0,:])
    q1 = ut.Quaternion(m[0,:], 1.0)

    # from matrix
    m1 = np.identity(4)
    p1 = ut.Pose(m1)

    #test accessors
    assert np.all(p1.translation() == np.asarray([0,0,0]))
    #assert p1.rotation() == np.array(0,0,0)

    #assert p == p1

    p2 = p * p1
    p3 = p.invert()

