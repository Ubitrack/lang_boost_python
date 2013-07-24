/*
 * UbiTrack - Advanced Facade Example
 * by Ulrich Eck <ulrich.eck@magicvisionlab.com>
 *
 */

#include <stdlib.h>
#include <signal.h>
#include <iostream>
#ifdef _WIN32
#include <conio.h>
#endif
#include <boost/thread.hpp>
#include <boost/program_options.hpp>


#include <utFacade/AdvancedFacade.h>
#include <utUtil/Exception.h>
#include <utUtil/Logging.h>
#include <utUtil/OS.h>

#include <utMeasurement/Measurement.h>
#include <utComponents/ApplicationPullSink.h>


#include <boost/bind.hpp>
#include <vector>

using Ubitrack;


Facade::AdvancedFacade* facade;
bool bStop = false;


void ctrlC ( int i )
{
        bStop = true;
}

void receivePose(Measurement::Pose& pose) {
	std::cout << "received pushed Pose: " << pose << std::endl;
}



int main(int argc, char **argv) {
	
	signal ( SIGINT, &ctrlC );

    try
    {
	    // initialize logging           
	    Util::initLogging();

	    // program options
	    std::string sUtqlFile;
	    std::string sComponentsPath;
	    bool bNoExit;

	    try
	    {
	           // describe program options
	           namespace po = boost::program_options;
	           po::options_description poDesc( "Allowed options", 80 );
	           poDesc.add_options()
	                   ( "help", "print this help message" )
	                   ( "components_path", po::value< std::string >( &sComponentsPath ), "Directory from which to load components" )
	                   ( "utql", po::value< std::string >( &sUtqlFile ), "UTQL request or response file, depending on whether a server is specified. "
	                           "Without specifying this option, the UTQL file can also be given directly on the command line." )
	                   ( "noexit", "do not exit on return" )
	                   #ifdef _WIN32
	                   ( "priority", po::value< int >( 0 ),"set priority of console thread, -1: lower, 0: normal, 1: higher, 2: real time (needs admin)" )
	                   #endif
	           ;
         
	           // specify default options
	           po::positional_options_description inputOptions;
	           inputOptions.add( "utql", 1 );          
         
	           // parse options from command line and environment
	           po::variables_map poOptions;
	           po::store( po::command_line_parser( ac, av ).options( poDesc ).positional( inputOptions ).run(), poOptions );
	           po::store( po::parse_environment( poDesc, "UBITRACK_" ), poOptions );
	           po::notify( poOptions );

	           #ifdef _WIN32
	           // set to quite high priority, see: http://msdn.microsoft.com/en-us/library/windows/desktop/ms686219%28v=vs.85%29.aspx
	           // carefully use this function under windows to steer ubitrack's cpu time:
	           if(poOptions.count("priority") != 0) {

	                int prio = poOptions["priority"].as<int>();
	                switch(prio){
	                case -1:
	                        SetPriorityClass( GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS );
	                        break;
	                case 0:
	                        //SetPriorityClass( GetCurrentProcess(), NORMAL_PRIORITY_CLASS  );
	                        break;
	                case 1:
	                        SetPriorityClass( GetCurrentProcess(), HIGH_PRIORITY_CLASS  );
	                        break;
	                case 2:
	                        SetPriorityClass( GetCurrentProcess(), REALTIME_PRIORITY_CLASS  );
	                        break;
	                }
	        }
	        #endif

	        bNoExit = poOptions.count( "noexit" ) != 0;

	        // print help message if nothing specified
	        if ( poOptions.count( "help" ) || sUtqlFile.empty() )
	        {
	                std::cout << "Syntax: utConsole [options] [--utql] <UTQL file>" << std::endl << std::endl;
	                std::cout << poDesc << std::endl;
	                return 1;
	        }
		}
		catch( std::exception& e )
		{
		        std::cerr << "Error parsing command line parameters : " << e.what() << std::endl;
		        std::cerr << "Try utConsole --help for help" << std::endl;
		        return 1;
		}

        // configure ubitrack
        std::cout << "Loading components..." << std::endl << std::flush;
        Facade::AdvancedFacade utFacade( sComponentsPath );

        std::cout << "Instantiating dataflow network from " << sUtqlFile << "..." << std::endl << std::flush;
        utFacade.loadDataflow( sUtqlFile );


		// connecting a pushsinkcallback
		facade->setCallback< Measurement::Pose >( "PushSinkPose", receivePose );
		
        Components::ApplicationPullSinkPose * pSink;

        try
        {
                 pSink = facade->componentByName< Components::ApplicationPullSinkPose >( "PullSinkPose" ).get();
        }
        catch ( const Ubitrack::Util::Exception& e )
        {
                std::cout << "Caught exception in SimpleFacade::getSimplePullSinkPose( " << sComponentName <<" ): " << e  << std::endl;
                setError( e.what() );
                return 1;
        }
		

        std::cout << "Starting dataflow" << std::endl;
        utFacade.startDataflow();
      	
		while( !bStop )
        {
			unsigned long long timestamp = Measurement::now();
			
			if (pSink != NULL) {
				// Retrieve measurement for current timestamp
	            Ubitrack::Measurement::ErrorPose measurement = pSink->get(timestamp);
				std::cout << "Sucessfully pulled pose in SimpleApplicationPullSinkPosePrivate::getPose(): " << measurement << std::endl;
			}
			
            Util::sleep( 1000 );
            #ifdef _WIN32
            if(kbhit())
            {
                    char c = getch();               
                    if(c == 'q') bStop = true;
            }
            #endif
        }

        std::cout << "Stopping dataflow..." << std::endl << std::flush;
        utFacade.stopDataflow();

        std::cout << "Finished, cleaning up..." << std::endl << std::flush;
	}
	catch( Util::Exception& e )
	{
	        std::cout << "exception occurred" << std::endl << std::flush;
	        std::cerr << e << std::endl;
	}

	std::cout << "utConsole terminated." << std::endl << std::flush;

	
	
	
}