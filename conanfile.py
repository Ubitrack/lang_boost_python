from conans import ConanFile, CMake
from conans import tools
from conans.tools import os_info, SystemPackageTool
import os, sys
import sysconfig
from io import StringIO

class UbitrackCoreConan(ConanFile):
    name = "ubitrack_lang_boost_python"
    version = "1.3.0"

    description = "Ubitrack Facade Python Bindings"
    url = "https://github.com/Ubitrack/lang_boost_python.git"
    license = "GPL"
    options = {"python": "ANY",}
    default_options = ("python=python",)

    short_paths = True
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    requires = (
        "Boost/[>=1.59.0,<1.63.0]@camposs/stable",
        "boost_numpy/0.8@camposs/stable", # only until we manage to upgrade to boost versions >= 1.63.0
        "ubitrack_core/%s@ubitrack/stable" % version,
        "ubitrack_vision/%s@ubitrack/stable" % version,
        "ubitrack_dataflow/%s@ubitrack/stable" % version,
        "ubitrack_hapticcalibration/%s@ubitrack/stable" % version,
        "ubitrack_facade/%s@ubitrack/stable" % version,
       )

    # all sources are deployed with the package
    exports_sources = "cmake/*", "include/*", "doc/*", "lib/*", "src/*", "tests/*", "CMakeLists.txt", "setup.py.in"

    def configure(self):
        self.options['Boost'].python = self.options.python
        self.options['Boost'].without_python = False

    def imports(self):
        self.copy(pattern="*.dll", dst="bin", src="bin") # From bin to bin
        self.copy(pattern="*.dylib*", dst="lib", src="lib") 
       
    def build(self):
        cmake = CMake(self)
        cmake.definitions['PYTHON_EXECUTABLE'] = self.python_exec.replace("\\", "/")
        cmake.definitions['PYTHON_VERSION_FULL'] = self.python_version
        cmake.definitions['PYTHON_NUMPY_INCLUDE_DIR'] = self.numpy_include
        cmake.configure()
        cmake.build()
        cmake.install()

    def package(self):
        # self.copy("*.h", dst="include", src="src")
        # self.copy("*.lib", dst="lib", excludes="*/lib/ubitrack/*.lib", keep_path=False)
        # self.copy("*.dll", dst="bin", excludes="*/lib/ubitrack/*.dll", keep_path=False)
        # self.copy("*.dylib*", dst="lib", excludes="*/lib/ubitrack/*.dylib", keep_path=False)
        # self.copy("*.so", dst="lib", excludes="*/lib/ubitrack/*.so", keep_path=False)
        # self.copy("*.a", dst="lib", keep_path=False)
        # self.copy("*", dst="bin", src="bin", keep_path=False)

        # # components
        # self.copy("ubitrack/*.lib", dst="lib/ubitrack", keep_path=False)
        # self.copy("ubitrack/*.dll", dst="bin/ubitrack", keep_path=False)
        # self.copy("ubitrack/*.dylib*", dst="lib/ubitrack", keep_path=False)
        # self.copy("ubitrack/*.so", dst="lib/ubitrack", keep_path=False)
        pass

    def package_info(self):
        pass



    @property
    def python_exec(self):
        try:
            pyexec = str(self.options.python)
            output = StringIO()
            self.run('{0} -c "import sys; print(sys.executable)"'.format(pyexec), output=output)
            return '"'+output.getvalue().strip().replace("\\","/")+'"'
        except:
            return ""
    
    _python_version = ""
    @property
    def python_version(self):
        cmd = "from sys import *; print('%d.%d' % (version_info[0],version_info[1]))"
        self._python_version = self._python_version or self.run_python_command(cmd)
        return self._python_version
      
    @property
    def python_include(self):
        pyinclude = self.get_python_path("include")
        if not os.path.exists(os.path.join(pyinclude, 'pyconfig.h')):
            return ""
        else:
            return pyinclude.replace('\\', '/')
    
    @property
    def python_lib(self):
        stdlib_dir = os.path.dirname(self.get_python_path("stdlib")).replace('\\', '/')
        return stdlib_dir
        
    @property
    def numpy_include(self):
        cmd = "import os; os.environ['DISTUTILS_USE_SDK']='1'; import numpy.distutils; print(numpy.distutils.misc_util.get_numpy_include_dirs()[0])" 
        return self.run_python_command(cmd)
                  
    def get_python_path(self, dir_name):
        cmd = "import sysconfig; print(sysconfig.get_path('{0}'))".format(dir_name)
        return self.run_python_command(cmd)   

    def run_python_command(self, cmd):
        pyexec = self.python_exec
        if pyexec:
            output = StringIO()
            self.run('{0} -c "{1}"'.format(pyexec, cmd), output=output)
            return output.getvalue().strip()
        else:
            return ""
