
#include "RinexClockStream.hpp"
#include "RinexClockHeader.hpp"
#include "RinexClockData.hpp"
#include "RinexClockFilterOperators.hpp"
#include "FileFilterFrameWithHeader.hpp"
#include "MergeFrame.hpp"


using namespace std;
using namespace gpstk;


class MergeRinClk : public MergeFrame{

   public:
      MergeRinClk( char *arg0)
         : MergeFrame( arg0,
                       std::string("RINEX Clk"),
                       std::string("Merge clk file") )
   {}


   protected:
      virtual void process();
};


void MergeRinClk::process(){

   std::vector<std::string> files = inputFileOption.getValue();

   FileFilterFrameWithHeader<RinexClockStream, RinexClockData, RinexClockHeader> 
      fff(files);

   RinexClockHeaderTouchHeaderMerge merged;
   fff.touchHeader(merged);

   fff.sort(RinexClockDataOperatorLessThanFull());
   fff.unique(RinexClockDataOperatorEquals());

   std::string outputFile = outputFileOption.getValue().front();
   fff.writeFile(outputFile, merged.theHeader);

}

int main(int argc, char *argv[]){

   try{

      MergeRinClk m(argv[0]);

      if( !m.initialize(argc, argv) ) return 0;
      if( !m.run() ) return 1;

      return 0;
   }catch( Exception& exc ){
      cout << exc << endl;
   }catch( exception& exc ){
      cout << exc.what() << endl;
   }catch(...){
      cout << "unknown error" << endl;
   }
   return 1;
}


