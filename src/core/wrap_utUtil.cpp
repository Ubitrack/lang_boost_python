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

using namespace Ubitrack;

namespace bp = boost::python;

namespace {

void translateException( const Ubitrack::Util::Exception& e) {
    PyErr_SetString(PyExc_RuntimeError, e.what());
};




/// A stream buffer getting data from and putting data into a Python file object
/** The aims are as follow:

    - Given a C++ function acting on a standard stream, e.g.

      \code
      void read_inputs(std::istream& input) {
        ...
        input >> something >> something_else;
      }
      \endcode

      and given a piece of Python code which creates a file-like object,
      to be able to pass this file object to that C++ function, e.g.

      \code
      import gzip
      gzip_file_obj = gzip.GzipFile(...)
      read_inputs(gzip_file_obj)
      \endcode

      and have the standard stream pull data from and put data into the Python
      file object.

    - When Python \c read_inputs() returns, the Python object is able to
      continue reading or writing where the C++ code left off.

    - Operations in C++ on mere files should be competitively fast compared
      to the direct use of \c std::fstream.


    \b Motivation

      - the standard Python library offer of file-like objects (files,
        compressed files and archives, network, ...) is far superior to the
        offer of streams in the C++ standard library and Boost C++ libraries.

      - i/o code involves a fair amount of text processing which is more
        efficiently prototyped in Python but then one may need to rewrite
        a time-critical part in C++, in as seamless a manner as possible.

    \b Usage

    This is 2-step:

      - a trivial wrapper function

        \code
          using ecto::python::streambuf;
          void read_inputs_wrapper(streambuf& input)
          {
            streambuf::istream is(input);
            read_inputs(is);
          }

          def("read_inputs", read_inputs_wrapper);
        \endcode

        which has to be written every time one wants a Python binding for
        such a C++ function.

      - the Python side

        \code
          from boost.python import streambuf
          read_inputs(streambuf(python_file_obj=obj, buffer_size=1024))
        \endcode

        \c buffer_size is optional. See also: \c default_buffer_size

  Note: references are to the C++ standard (the numbers between parentheses
  at the end of references are margin markers).
*/

inline
std::string
file_and_line_as_string(
  const char* file,
  long line)
{
  std::ostringstream o;
  o << file << "(" << line << ")";
  return o.str();
}

#define TBXX_ASSERT(condition) \
  if (!(condition)) { \
    throw std::runtime_error( \
      file_and_line_as_string( \
        __FILE__, __LINE__) \
      + ": ASSERT(" #condition ") failure."); \
  }

#define TBXX_UNREACHABLE_ERROR() \
  std::runtime_error( \
    "Control flow passes through branch that should be unreachable: " \
    + file_and_line_as_string(__FILE__, __LINE__))

class streambuf : public std::basic_streambuf<char>
{
  private:
    typedef std::basic_streambuf<char> base_t;

  public:
    /* The syntax
        using base_t::char_type;
       would be nicer but Visual Studio C++ 8 chokes on it
    */
    typedef base_t::char_type   char_type;
    typedef base_t::int_type    int_type;
    typedef base_t::pos_type    pos_type;
    typedef base_t::off_type    off_type;
    typedef base_t::traits_type traits_type;

    // work around Visual C++ 7.1 problem
    inline static int
    traits_type_eof() { return traits_type::eof(); }

    /// The default size of the read and write buffer.
    /** They are respectively used to buffer data read from and data written to
        the Python file object. It can be modified from Python.
    */
    static std::size_t default_buffer_size;

    /// Construct from a Python file object
    /** if buffer_size is 0 the current default_buffer_size is used.
    */
    streambuf(
      bp::object& python_file_obj,
      std::size_t buffer_size_=0)
    :
      py_read (getattr(python_file_obj, "read",  bp::object())),
      py_write(getattr(python_file_obj, "write", bp::object())),
      py_seek (getattr(python_file_obj, "seek",  bp::object())),
      py_tell (getattr(python_file_obj, "tell",  bp::object())),
      buffer_size(buffer_size_ != 0 ? buffer_size_ : default_buffer_size),
      write_buffer(0),
      pos_of_read_buffer_end_in_py_file(0),
      pos_of_write_buffer_end_in_py_file(buffer_size),
      farthest_pptr(0),
      file_obj(python_file_obj)
    {
      TBXX_ASSERT(buffer_size != 0);
      /* Some Python file objects (e.g. sys.stdout and sys.stdin)
         have non-functional seek and tell. If so, assign None to
         py_tell and py_seek.
       */
      if (py_tell != bp::object()) {
        try {
          py_tell();
        }
        catch (bp::error_already_set&) {
          py_tell = bp::object();
          py_seek = bp::object();
          /* Boost.Python does not do any Python exception handling whatsoever
             So we need to catch it by hand like so.
           */
          PyErr_Clear();
        }
      }

      if (py_write != bp::object()) {
        // C-like string to make debugging easier
        write_buffer = new char[buffer_size + 1];
        write_buffer[buffer_size] = '\0';
        setp(write_buffer, write_buffer + buffer_size);  // 27.5.2.4.5 (5)
        farthest_pptr = pptr();
      }
      else {
        // The first attempt at output will result in a call to overflow
        setp(0, 0);
      }

      if (py_tell != bp::object()) {
        off_type py_pos = bp::extract<off_type>(py_tell());
        pos_of_read_buffer_end_in_py_file = py_pos;
        pos_of_write_buffer_end_in_py_file = py_pos;
      }
    }

    /// Mundane destructor freeing the allocated resources
    virtual ~streambuf() {
      if (write_buffer) delete[] write_buffer;
    }

    /// C.f. C++ standard section 27.5.2.4.3
    /** It is essential to override this virtual function for the stream
        member function readsome to work correctly (c.f. 27.6.1.3, alinea 30)
     */
    virtual std::streamsize showmanyc() {
      int_type const failure = traits_type::eof();
      int_type status = underflow();
      if (status == failure) return -1;
      return egptr() - gptr();
    }

    /// C.f. C++ standard section 27.5.2.4.3
    virtual int_type underflow() {
      int_type const failure = traits_type::eof();
      if (py_read == bp::object()) {
        throw std::invalid_argument(
          "That Python file object has no 'read' attribute");
      }
      read_buffer = py_read(buffer_size);
      char *read_buffer_data;
      bp::ssize_t py_n_read;
      if (PyString_AsStringAndSize(read_buffer.ptr(),
                                   &read_buffer_data, &py_n_read) == -1) {
        setg(0, 0, 0);
        throw std::invalid_argument(
          "The method 'read' of the Python file object "
          "did not return a string.");
      }
      off_type n_read = (off_type)py_n_read;
      pos_of_read_buffer_end_in_py_file += n_read;
      setg(read_buffer_data, read_buffer_data, read_buffer_data + n_read);
      // ^^^27.5.2.3.1 (4)
      if (n_read == 0) return failure;
      return traits_type::to_int_type(read_buffer_data[0]);
    }

    /// C.f. C++ standard section 27.5.2.4.5
    virtual int_type overflow(int_type c=traits_type_eof()) {
      if (py_write == bp::object()) {
        throw std::invalid_argument(
          "That Python file object has no 'write' attribute");
      }
      farthest_pptr = std::max(farthest_pptr, pptr());
      off_type n_written = (off_type)(farthest_pptr - pbase());
      bp::str chunk(pbase(), farthest_pptr);
      py_write(chunk);
      if (!traits_type::eq_int_type(c, traits_type::eof())) {
        py_write(traits_type::to_char_type(c));
        n_written++;
      }
      if (n_written) {
        pos_of_write_buffer_end_in_py_file += n_written;
        setp(pbase(), epptr());
        // ^^^ 27.5.2.4.5 (5)
        farthest_pptr = pptr();
      }
      return traits_type::eq_int_type(
        c, traits_type::eof()) ? traits_type::not_eof(c) : c;
    }

    /// Update the python file to reflect the state of this stream buffer
    /** Empty the write buffer into the Python file object and set the seek
        position of the latter accordingly (C++ standard section 27.5.2.4.2).
        If there is no write buffer or it is empty, but there is a non-empty
        read buffer, set the Python file object seek position to the
        seek position in that read buffer.
    */
    virtual int sync() {
      int result = 0;
      farthest_pptr = std::max(farthest_pptr, pptr());
      if (farthest_pptr && farthest_pptr > pbase()) {
        off_type delta = pptr() - farthest_pptr;
        int_type status = overflow();
        if (traits_type::eq_int_type(status, traits_type::eof())) result = -1;
        if (py_seek != bp::object()) py_seek(delta, 1);
      }
      else if (gptr() && gptr() < egptr()) {
        if (py_seek != bp::object()) py_seek(gptr() - egptr(), 1);
      }
      return result;
    }

    /// C.f. C++ standard section 27.5.2.4.2
    /** This implementation is optimised to look whether the position is within
        the buffers, so as to avoid calling Python seek or tell. It is
        important for many applications that the overhead of calling into Python
        is avoided as much as possible (e.g. parsers which may do a lot of
        backtracking)
    */
    virtual
    pos_type seekoff(off_type off, std::ios_base::seekdir way,
                     std::ios_base::openmode which=  std::ios_base::in
                                                   | std::ios_base::out)
    {
      /* In practice, "which" is either std::ios_base::in or out
         since we end up here because either seekp or seekg was called
         on the stream using this buffer. That simplifies the code
         in a few places.
      */
      int const failure = off_type(-1);

      if (py_seek == bp::object()) {
        throw std::invalid_argument(
          "That Python file object has no 'seek' attribute");
      }

      // we need the read buffer to contain something!
      if (which == std::ios_base::in && !gptr()) {
        if (traits_type::eq_int_type(underflow(), traits_type::eof())) {
          return failure;
        }
      }

      // compute the whence parameter for Python seek
      int whence;
      switch (way) {
        case std::ios_base::beg:
          whence = 0;
          break;
        case std::ios_base::cur:
          whence = 1;
          break;
        case std::ios_base::end:
          whence = 2;
          break;
        default:
          return failure;
      }

      // Let's have a go
      boost::optional<off_type> result = seekoff_without_calling_python(
        off, way, which);
      if (!result) {
        // we need to call Python
        if (which == std::ios_base::out) overflow();
        if (way == std::ios_base::cur) {
          if      (which == std::ios_base::in)  off -= egptr() - gptr();
          else if (which == std::ios_base::out) off += pptr() - pbase();
        }
        py_seek(off, whence);
        result = off_type(bp::extract<off_type>(py_tell()));
        if (which == std::ios_base::in) underflow();
      }
      return *result;
    }

    /// C.f. C++ standard section 27.5.2.4.2
    virtual
    pos_type seekpos(pos_type sp,
                     std::ios_base::openmode which=  std::ios_base::in
                                                   | std::ios_base::out)
    {
      return streambuf::seekoff(sp, std::ios_base::beg, which);
    }


  private:
    bp::object py_read, py_write, py_seek, py_tell;

    std::size_t buffer_size;

    /* This is actually a Python string and the actual read buffer is
       its internal data, i.e. an array of characters. We use a Boost.Python
       object so as to hold on it: as a result, the actual buffer can't
       go away.
    */
    bp::object read_buffer;

    /* A mere array of char's allocated on the heap at construction time and
       de-allocated only at destruction time.
    */
    char *write_buffer;

    off_type pos_of_read_buffer_end_in_py_file,
             pos_of_write_buffer_end_in_py_file;

    // the farthest place the buffer has been written into
    char *farthest_pptr;


    boost::optional<off_type> seekoff_without_calling_python(
      off_type off,
      std::ios_base::seekdir way,
      std::ios_base::openmode which)
    {
      boost::optional<off_type> const failure;

      // Buffer range and current position
      off_type buf_begin, buf_end, buf_cur, upper_bound;
      off_type pos_of_buffer_end_in_py_file;
      if (which == std::ios_base::in) {
        pos_of_buffer_end_in_py_file = pos_of_read_buffer_end_in_py_file;
        buf_begin = reinterpret_cast<std::streamsize>(eback());
        buf_cur = reinterpret_cast<std::streamsize>(gptr());
        buf_end = reinterpret_cast<std::streamsize>(egptr());
        upper_bound = buf_end;
      }
      else if (which == std::ios_base::out) {
        pos_of_buffer_end_in_py_file = pos_of_write_buffer_end_in_py_file;
        buf_begin = reinterpret_cast<std::streamsize>(pbase());
        buf_cur = reinterpret_cast<std::streamsize>(pptr());
        buf_end = reinterpret_cast<std::streamsize>(epptr());
        farthest_pptr = std::max(farthest_pptr, pptr());
        upper_bound = reinterpret_cast<std::streamsize>(farthest_pptr) + 1;
      }
      else {
        throw TBXX_UNREACHABLE_ERROR();
      }

      // Sought position in "buffer coordinate"
      off_type buf_sought;
      if (way == std::ios_base::cur) {
        buf_sought = buf_cur + off;
      }
      else if (way == std::ios_base::beg) {
        buf_sought = buf_end + (off - pos_of_buffer_end_in_py_file);
      }
      else if (way == std::ios_base::end) {
        return failure;
      }
      else {
        throw TBXX_UNREACHABLE_ERROR();
      }

      // if the sought position is not in the buffer, give up
      if (buf_sought < buf_begin || buf_sought >= upper_bound) return failure;

      // we are in wonderland
      if      (which == std::ios_base::in)  gbump(buf_sought - buf_cur);
      else if (which == std::ios_base::out) pbump(buf_sought - buf_cur);
      return pos_of_buffer_end_in_py_file + (buf_sought - buf_end);
    }

  public:

    class istream : public std::istream
    {
      public:
        istream(streambuf& buf) : std::istream(&buf)
        {
          exceptions(std::ios_base::badbit);
        }

        ~istream() { if (this->good()) this->sync(); }
    };

    class ostream : public std::ostream
    {
      public:
        ostream(streambuf& buf) : std::ostream(&buf)
        {
          exceptions(std::ios_base::badbit);
        }

        ~ostream() { if (this->good()) this->flush(); }
    };
    bp::object file_obj; //original handle
};

std::size_t streambuf::default_buffer_size = 1024;

struct streambuf_capsule
{
  streambuf python_streambuf;

  streambuf_capsule(
    bp::object& python_file_obj,
    std::size_t buffer_size=0)
  :
    python_streambuf(python_file_obj, buffer_size)
  {}

  bp::object get_original_file() const {return python_streambuf.file_obj;}
};

struct ostream : streambuf_capsule, streambuf::ostream
{
  ostream(
    bp::object& python_file_obj,
    std::size_t buffer_size=0)
  :
    streambuf_capsule(python_file_obj, buffer_size),
    streambuf::ostream(python_streambuf)
  {}

  ~ostream()
  {
    try {
      if (this->good()) this->flush();
    }
    catch (bp::error_already_set&) {
      PyErr_Clear();
    // fixme this should not throw in the detructor
    //  throw std::runtime_error(
      std::cerr << (
        "Problem closing python ostream.\n"
        "  Known limitation: the error is unrecoverable. Sorry.\n"
        "  Suggestion for programmer: add ostream.flush() before"
        " returning.") << std::endl;
    }
  }
};

struct istream : streambuf_capsule, streambuf::istream
{
  istream(
    bp::object& python_file_obj,
    std::size_t buffer_size=0)
  :
    streambuf_capsule(python_file_obj, buffer_size),
    streambuf::istream(python_streambuf)
  {}
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
			std::cout << "EventStreamReader exception: " << ex.what() << std::endl;
		} catch (const std::string& ex) {
			std::cout << "EventStreamReader string exception: " << ex << std::endl;
		} catch (...) {
			std::cout << "EventStreamReader unknown exception" << std::endl;
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
    os.def_readwrite("file",&ostream::get_original_file);

    bp::class_<std::istream, boost::shared_ptr<std::istream>, boost::noncopyable>("std_istream", bp::no_init);
    bp::class_<istream, boost::noncopyable, bp::bases<std::istream> > is("istream", bp::no_init);
    is.def(bp::init<bp::object&, std::size_t>((bp::arg("python_file_obj"), bp::arg("buffer_size") = 0)));
    is.def_readwrite("file",&ostream::get_original_file);


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


    bp::def("readCalibDistance", &readUtCalibFile< Measurement::Distance >
//		,bp::return_value_policy<bp::manage_new_object>()
	);
    bp::def("readCalibPose", &readUtCalibFile< Measurement::Pose >
//		,bp::return_value_policy<bp::manage_new_object>()
	);
    bp::def("readCalibErrorPose", &readUtCalibFile< Measurement::ErrorPose >
//		,bp::return_value_policy<bp::manage_new_object>()
	);
    bp::def("readCalibErrorPosition", &readUtCalibFile< Measurement::ErrorPosition >
//		,bp::return_value_policy<bp::manage_new_object>()
	);
    bp::def("readCalibPosition", &readUtCalibFile< Measurement::Position >
//		,bp::return_value_policy<bp::manage_new_object>()
	);
    bp::def("readCalibRotation", &readUtCalibFile< Measurement::Rotation >
//		,bp::return_value_policy<bp::manage_new_object>()
	);
    bp::def("readCalibMatrix3x3", &readUtCalibFile< Measurement::Matrix3x3 >
//		,bp::return_value_policy<bp::manage_new_object>()
	);
    bp::def("readCalibMatrix3x4", &readUtCalibFile< Measurement::Matrix3x4 >
//		,bp::return_value_policy<bp::manage_new_object>()
	);
    bp::def("readCalibMatrix4x4", &readUtCalibFile< Measurement::Matrix4x4 >
//		,bp::return_value_policy<bp::manage_new_object>()
	);
    bp::def("readCalibVector4D", &readUtCalibFile< Measurement::Vector4D >
//		,bp::return_value_policy<bp::manage_new_object>()
	);
    bp::def("readCalibVector8D", &readUtCalibFile< Measurement::Vector8D >
//		,bp::return_value_policy<bp::manage_new_object>()
	);
    bp::def("readCalibPositionList", &readUtCalibFile< Measurement::PositionList >
//		,bp::return_value_policy<bp::manage_new_object>()
	);
    bp::def("readCalibPositionList2", &readUtCalibFile< Measurement::PositionList2 >
//		,bp::return_value_policy<bp::manage_new_object>()
	);
    bp::def("readCalibPoseList", &readUtCalibFile< Measurement::PoseList >
//		,bp::return_value_policy<bp::manage_new_object>()
	);
    bp::def("readCalibDistanceList", &readUtCalibFile< Measurement::DistanceList >
//		,bp::return_value_policy<bp::manage_new_object>()
	);


}
