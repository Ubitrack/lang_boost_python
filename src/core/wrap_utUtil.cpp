#include <boost/python.hpp>
#include <boost/python/implicit.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/to_python_converter.hpp>

#include <boost/python/object.hpp>
#include <boost/python/str.hpp>
#include <boost/python/extract.hpp>

#include <boost/optional.hpp>
#include <boost/utility/typed_in_place_factory.hpp>

#include <boost/filesystem.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>

#include <istream>
#include <streambuf>
#include <iostream>

#include <utMeasurement/Measurement.h>

#include <utUtil/Logging.h>
#include <utUtil/Exception.h>
#include <utUtil/CalibFile.h>

#include <ubitrack_python/wrap_streambuf.h>

using namespace Ubitrack;
using namespace Ubitrack::Python;

namespace bp = boost::python;

namespace {

void translateException( const Ubitrack::Util::Exception& e) {
    PyErr_SetString(PyExc_RuntimeError, e.what());
};







// read calib files
template< class T >
typename T::value_type readUtCalibFile(const std::string& sFile) {
	typename T::value_type value;
	Util::readCalibFileDropMeasurement<typename T::value_type>(sFile, value);
	return value;
}


} // end anon ns


template< class EventType >
class EventStreamReader {
public:
	EventStreamReader( streambuf& sb) {
		streambuf::istream file(sb);

		try {
			boost::archive::text_iarchive archive( file );

			// could be improved by only loading the requested event instead of all during init.
			// read contents until end-of-file exception
			try
			{
				while ( true )
				{
					EventType e( boost::shared_ptr< typename EventType::value_type >( new typename EventType::value_type() ) );
					std::string dummy; // for newline character in archive
					archive >> dummy >> e;
					data.append( e );
				}
			}
			catch (const std::exception& ex) {
				// end of archive
				if (std::string(ex.what()).compare("input stream error") != 0 ) {
					PyErr_SetString(PyExc_RuntimeError, ex.what());
				}
			} catch (const std::string& ex) {
				PyErr_SetString(PyExc_RuntimeError, ex.c_str());
			} catch (...) {
				PyErr_SetString(PyExc_RuntimeError, "EventStreamReader unknown exception");
			}

		}
		catch (const std::exception& ex) {
			PyErr_SetString(PyExc_RuntimeError, ex.what());
		} catch (const std::string& ex) {
			PyErr_SetString(PyExc_RuntimeError, ex.c_str());
		} catch (...) {
			PyErr_SetString(PyExc_RuntimeError, "Init iarchive unknown exception");
		}

	}
	virtual ~EventStreamReader() {}

	bp::list values() {
		return data;
	}

protected:
	bp::list data;
};

BOOST_PYTHON_FUNCTION_OVERLOADS(logging_overloads, Ubitrack::Util::initLogging, 0, 1)

BOOST_PYTHON_MODULE(_ututil)
{
	bp::register_exception_translator<Ubitrack::Util::Exception>(translateException);

	bp::def("initLogging", &Ubitrack::Util::initLogging, logging_overloads());


	// provide streambuf support
	bp::class_<streambuf, boost::shared_ptr<streambuf>, boost::noncopyable> sb("streambuf", bp::no_init);
    sb.def(bp::init<bp::object&, std::size_t>((bp::arg("file"), bp::arg("buffer_size") = 0)));
    sb.def_readwrite("default_buffer_size", streambuf::default_buffer_size, "The default size of the buffer sitting "
                     "between a Python file object and a C++ stream.");

    bp::class_<std::ostream, boost::shared_ptr<std::ostream>, boost::noncopyable>("std_ostream", bp::no_init);
    bp::class_<ostream, boost::noncopyable, bp::bases<std::ostream> > os("ostream", bp::no_init);
    os.def(bp::init<bp::object&, std::size_t>((bp::arg("python_file_obj"), bp::arg("buffer_size") = 0)));
	// XXX fails on windows VS2010
    //os.def_readwrite("file",&ostream::get_original_file);

    bp::class_<std::istream, boost::shared_ptr<std::istream>, boost::noncopyable>("std_istream", bp::no_init);
    bp::class_<istream, boost::noncopyable, bp::bases<std::istream> > is("istream", bp::no_init);
    is.def(bp::init<bp::object&, std::size_t>((bp::arg("python_file_obj"), bp::arg("buffer_size") = 0)));
	// XXX fails on windows VS2010
    //is.def_readwrite("file",&ostream::get_original_file);


    // EventStreamReaders
    bp::class_< EventStreamReader< Measurement::Pose > >("PoseStreamReader", bp::init< streambuf& >())
		.def("values", &EventStreamReader<Measurement::Pose >::values)
		;

    bp::class_< EventStreamReader< Measurement::ErrorPose > >("ErrorPoseStreamReader", bp::init< streambuf& >())
		.def("values", &EventStreamReader<Measurement::ErrorPose >::values)
		;

    bp::class_< EventStreamReader< Measurement::ErrorPosition > >("ErrorPositionStreamReader", bp::init< streambuf& >())
		.def("values", &EventStreamReader<Measurement::ErrorPosition >::values)
		;

    bp::class_< EventStreamReader< Measurement::Rotation > >("RotationStreamReader", bp::init< streambuf& >())
		.def("values", &EventStreamReader<Measurement::Rotation >::values)
		;

    bp::class_< EventStreamReader< Measurement::Position > >("PositionStreamReader", bp::init< streambuf& >())
		.def("values", &EventStreamReader<Measurement::Position >::values)
		;

    bp::class_< EventStreamReader< Measurement::Position2D > >("Position2DStreamReader", bp::init< streambuf& >())
		.def("values", &EventStreamReader<Measurement::Position2D >::values)
		;

    bp::class_< EventStreamReader< Measurement::Matrix3x3 > >("Matrix3x3StreamReader", bp::init< streambuf& >())
		.def("values", &EventStreamReader<Measurement::Matrix3x3 >::values)
		;

    bp::class_< EventStreamReader< Measurement::Matrix3x4 > >("Matrix3x4StreamReader", bp::init< streambuf& >())
		.def("values", &EventStreamReader<Measurement::Matrix3x4 >::values)
		;

    bp::class_< EventStreamReader< Measurement::Matrix4x4 > >("Matrix4x4StreamReader", bp::init< streambuf& >())
		.def("values", &EventStreamReader<Measurement::Matrix4x4 >::values)
		;

    bp::class_< EventStreamReader< Measurement::PositionList > >("PositionListStreamReader", bp::init< streambuf& >())
		.def("values", &EventStreamReader<Measurement::PositionList >::values)
		;

    bp::class_< EventStreamReader< Measurement::PositionList2 > >("PositionList2StreamReader", bp::init< streambuf& >())
		.def("values", &EventStreamReader<Measurement::PositionList2 >::values)
		;

    bp::class_< EventStreamReader< Measurement::PoseList > >("PoseListStreamReader", bp::init< streambuf& >())
		.def("values", &EventStreamReader<Measurement::PoseList >::values)
		;

//    bp::class_< EventStreamReader< Measurement::RotationVelocity > >("RotationVelocityStreamReader", bp::init< streambuf& >())
//		.def("values", &EventStreamReader<Measurement::RotationVelocity >::values)
//		;
//
//    bp::class_< EventStreamReader< Measurement::Image > >("ImageStreamReader", bp::init< streambuf& >())
//		.def("values", &EventStreamReader<Measurement::Image >::values)
//		;


    bp::def("readCalibDistance", &readUtCalibFile< Measurement::Distance >);
    bp::def("readCalibPose", &readUtCalibFile< Measurement::Pose >);
    bp::def("readCalibErrorPose", &readUtCalibFile< Measurement::ErrorPose >);
    bp::def("readCalibErrorPosition", &readUtCalibFile< Measurement::ErrorPosition >);
    bp::def("readCalibPosition", &readUtCalibFile< Measurement::Position >);
    bp::def("readCalibRotation", &readUtCalibFile< Measurement::Rotation >);
    bp::def("readCalibMatrix3x3", &readUtCalibFile< Measurement::Matrix3x3 >);
    bp::def("readCalibMatrix3x4", &readUtCalibFile< Measurement::Matrix3x4 >);
    bp::def("readCalibMatrix4x4", &readUtCalibFile< Measurement::Matrix4x4 >);
    bp::def("readCalibVector4D", &readUtCalibFile< Measurement::Vector4D >);
    bp::def("readCalibVector8D", &readUtCalibFile< Measurement::Vector8D >);
    bp::def("readCalibPositionList", &readUtCalibFile< Measurement::PositionList >);
    bp::def("readCalibPositionList2", &readUtCalibFile< Measurement::PositionList2 >);
    bp::def("readCalibPoseList", &readUtCalibFile< Measurement::PoseList >);
    bp::def("readCalibDistanceList", &readUtCalibFile< Measurement::DistanceList >);


}
