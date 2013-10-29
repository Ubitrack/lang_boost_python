__author__ = 'jack'

from nose import with_setup

from ubitrack.core import math as utmath
import pyublas # load converters
import numpy as np
import math

import unittest


def setup_func():
    "set up test fixtures"
    pass

def teardown_func():
    "tear down test fixtures"
    pass

@with_setup(setup_func, teardown_func)
def test_basic_datatypes():
    "test basic data types: Vector2-8, Matrix33-44, Pose, Quaternion"

    q = utmath.test_quat()
    assert q.x() == 0 and q.y() == 0 and q.z() == 0 and q.w() == 1

    m = utmath.test_mat33()
    assert np.all(m == np.identity(3))

    # from vector
    p = utmath.Pose(q, m[0,:])
    q1 = utmath.Quaternion(m[0,:], 1.0)

    # from matrix
    m1 = np.identity(4)
    p1 = utmath.Pose(m1)

    #test accessors
    assert np.all(p1.translation() == np.asarray([0,0,0]))
    #assert p1.rotation() == np.array(0,0,0)

    #assert p == p1

    p2 = p * p1
    p3 = p.invert()



class test_quaternion( unittest.TestCase):


    def setUp( self ):
        pass

    def tearDown( self ):
        pass

    def test_create_identity( self ):
        result = utmath.Quaternion().toVector()

        expected = np.array( [ 0.0, 0.0, 0.0, 1.0 ] )

        self.assertTrue(
            np.array_equal( result, expected ),
            "Quaternion identity incorrect"
            )

    def test_normalise( self ):
        def identity():
            # normalise an identity quaternion
            quat = utmath.Quaternion()
            quat.normalize()

            expected = np.array( [ 0.0, 0.0, 0.0, 1.0 ] )
#             assert np.array_equal(
#                 expected,
#                 quat / math.sqrt( np.sum( quat ** 2 ) )
#                 )

            self.assertTrue(
                np.array_equal( quat.toVector(), expected ),
                "Normalise identity quaternion incorrect"
                )
        identity()
    
        def non_identity():
            # normalise a quaternion of length 2.0
            quat = np.array( [ 1.0, 2.0, 3.0, 4.0 ] )
            result = utmath.Quaternion.fromVector(quat)
            result.normalize()

            expected = quat / math.sqrt( np.sum( quat ** 2 ) )

            # check the length is 1.0
            self.assertTrue(
                np.array_equal( result.toVector(), expected ),
                "Normalise quaternion incorrect"
                )
        non_identity()

    def test_length( self ):
        def identity():
            quat = utmath.Quaternion()
            result = utmath.abs(quat)

            expected = 1.0

            self.assertEqual(
                result,
                expected,
                "Identity quaternion length calculation incorrect"
                )
        identity()

        def non_identity():
            quat = np.array( [ 1.0, 2.0, 3.0, 4.0 ] )
            result = utmath.abs(utmath.Quaternion.fromVector(quat))

            expected = math.sqrt( np.sum( quat ** 2 ) )
            self.assertEqual(
                result,
                expected,
                "Quaternion length calculation incorrect"
                )
        non_identity()


    def test_apply_to_vector( self ):
        # Euler angle constructor uses X-Z-Y ordering
        
        def identity():
            quat = utmath.Quaternion()
            vec = np.array( [ 1.0, 0.0, 0.0 ] )

            result = quat * vec

            expected = vec

            self.assertTrue(
                np.array_equal( result, expected ),
                "Quaternion apply_to_vector incorrect with identity"
                )
        identity()

        def rotated_x():
            quat = utmath.Quaternion(  0.0, 0.0, math.pi )
            vec = np.array( [ 0.0, 1.0, 0.0 ] )

            result = quat * vec

            expected = -vec

            self.assertTrue(
                np.allclose( result, expected ),
                "Quaternion apply_to_vector incorrect with rotation about X"
                )
        rotated_x()

        def rotated_y():
            quat = utmath.Quaternion( 0.0, math.pi, 0.0 )
            vec = np.array( [ 1.0, 0.0, 0.0 ] )

            result = quat * vec

            expected = -vec

            self.assertTrue(
                np.allclose( result, expected ),
                "Quaternion apply_to_vector incorrect with rotation about Y"
                )
        rotated_y()

        def rotated_z():
            quat = utmath.Quaternion( math.pi, 0.0, 0.0 )
            vec = np.array( [ 1.0, 0.0, 0.0 ] )

            result = quat * vec

            expected = -vec

            self.assertTrue(
                np.allclose( result, expected ),
                "Quaternion apply_to_vector incorrect with rotation about Y"
                )
        rotated_z()
