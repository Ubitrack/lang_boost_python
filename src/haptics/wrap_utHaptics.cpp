#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <boost/numpy.hpp>
#include <numpy/arrayobject.h>
#include <complex>

#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/to_python_converter.hpp>

#include <utHaptics/Function/PhantomForwardKinematics.h>
#include <utHaptics/PhantomLMCalibration.h>
#include <utHaptics/PhantomLMGimbalCalibration.h>

using namespace Ubitrack;

namespace bp = boost::python;
namespace bn = boost::numpy;

namespace {

    Math::Matrix< double, 3, 3 > computePhantomLMCalibration( const std::vector< Math::Vector< double, 3 > > &jointangles, const std::vector< Math::Vector< double, 3 > > &points,
            const double l1, const double l2, Math::Vector< double, 3 > &calib, const double optimizationStepSize, const double optimizationStepFactor ) {
        return Haptics::computePhantomLMCalibration(jointangles, points, l1, l2, calib, optimizationStepSize, optimizationStepFactor);
    }

    Math::Matrix< double, 3, 3 > computePhantomLMGimbalCalibration( const std::vector< Math::Vector< double, 3 > > & jointangles,
            const std::vector< Math::Vector< double, 3 > > & gimbalangles,
            const std::vector< Math::Vector< double, 3 > > & zref,
            const double l1,
            const double l2,
            const Math::Matrix< double, 3 , 3 > & angle_correction,
            const Math::Vector< double, 3 > & calib,
            const double optimizationStepSize,
            const double optimizationStepFactor) {
        return Haptics::computePhantomLMGimbalCalibration(jointangles, gimbalangles, zref, l1, l2, angle_correction, calib, optimizationStepSize, optimizationStepFactor);
    }

}

BOOST_PYTHON_MODULE(_uthaptics)
{
    bp::def("computePhantomLMCalibration", &computePhantomLMCalibration);
    bp::def("computePhantomLMGimbalCalibration", &computePhantomLMGimbalCalibration);
    bp::def("computePhantomForwardKinematicsPose", &Haptics::Function::computePhantomForwardKinematicsPose);
}
