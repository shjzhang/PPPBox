

#include "BasicFramework.hpp"
#include "CommandOption.hpp"

using namespace std;
using namespace gpstk;

class CalFrame : public gpstk::BasicFramework
{
public:

   CalFrame(char* arg0,
            const std::string& type,
            const std::string& message = std::string())
      : gpstk::BasicFramework(arg0,
                                 "Calculate " + type + message),

      coordsFileOption ( 'c', "coords",
            " [-c|--coordinates] Name of coordinates file", true),

      tempFileOption ( 't', "temp",
            " [-t|--temperature] Name of temperature file", true),

      presFileOption ( 'p', "pres",
            " [-p|--pressure] Name of pressure file", true),

      rhumFileOption ( 'r', "rhumid",
            " [-r|--rhumid] Name of relative humidity file", true),

      heightFileOption ( 'a', "altitude",
            " [-a|--altitude] Name of altitude file", true)


      {
         coordsFileOption.setMaxCount(1);
         tempFileOption.setMaxCount(1);
         presFileOption.setMaxCount(1);
         rhumFileOption.setMaxCount(1);
         heightFileOption.setMaxCount(1);

      }
      

      virtual bool initialize(int argc, char* argv[]) throw()
      {
         if ( !gpstk::BasicFramework::initialize(argc,argv) )
            return false;

         if ( coordsFileOption.getCount() != 1)
         {
            std::cerr << "This program requires one coordinates files."
                      << std::endl;
            return false;
         }

         std::string cfilename = coordsFileOption.getValue()[0];

         if ( tempFileOption.getCount() != 1)
         {
            std::cerr << "This program requires one temperature files."
                      << std::endl;
            return false;
         }

         std::string tfilename = tempFileOption.getValue()[0];

         if ( presFileOption.getCount() != 1)
         {
            std::cerr << "This program requires one pressure files."
                      << std::endl;
            return false;
         }

         std::string pfilename = presFileOption.getValue()[0];

         if ( rhumFileOption.getCount() != 1)
         {
            std::cerr << "This program requires one relative humidity files."
                      << std::endl;
            return false;
         }

         std::string rfilename = rhumFileOption.getValue()[0];

         if ( heightFileOption.getCount() != 1)
         {
            std::cerr << "This program requires one  height files."
                      << std::endl;
            return false;
         }

         std::string hfilename = heightFileOption.getValue()[0];
      }

protected:

      virtual void process() = 0;
      gpstk::CommandOptionWithAnyArg coordsFileOption;
      gpstk::CommandOptionWithAnyArg tempFileOption;
      gpstk::CommandOptionWithAnyArg presFileOption;
      gpstk::CommandOptionWithAnyArg rhumFileOption;
      gpstk::CommandOptionWithAnyArg heightFileOption;
};














