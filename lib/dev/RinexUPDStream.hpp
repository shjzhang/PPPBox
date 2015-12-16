#pragma ident "$Id: RinexUPDStream.hpp 2635 2012-08-12 12:56:47Z shjzhang $"

/**
 * @file RinexUPDStream.hpp
 * gpstk::RinexUPDStream - Satellite bias file stream
 */

#ifndef GPSTK_RINEX_UPD_STREAM_INCLUDE
#define GPSTK_RINEX_UPD_STREAM_INCLUDE

#include <iostream>
#include <fstream>

#include "FFTextStream.hpp"

namespace gpstk
{
   /// @addtogroup RinexClock
   //@{

       /** This class performs file I/O on an RINEX Clock format file for the
        *  RinexBiasHeader and RinexBiasData classes. 
        *
        *  The satellite bias data will be stored in the file with the format 
        *  similar as that of the rinex clock data. 
        *
        *  The header of bias file is the same as that of the rinex clock file.
        *  There are slight difference between the data record of the satellite
        *  bias file and rinex clock file.
        *
        *  The clock bias and sigma data will be replaced by the widelane and
        *  narrow-lane fractional-cycle biases data, and the data length also
        *  will be changed to be shorter.
        *
        *  \Warning The unit of the satellite biases is meter.
        */
   class RinexUPDStream : public FFTextStream
   {
   public:

         /// Default constructor
      RinexUPDStream() 
          : headerRead(false)
      {}
      
         /*  Common constructor: open (default: read mode)
          *
          *  @param filename the name of the ASCII RinexClock format file to be opened
          *  @param mode the ios::openmode to be used
          *
          */
      RinexUPDStream(const char* filename, std::ios::openmode mode=std::ios::in) 
          : FFTextStream(filename, mode), headerRead(false)
      {}


         /* override open() to reset the header
          * @param filename the name of the ASCII RINEX Clock format file
          * @param mode the ios::openmode to be used
          */
      virtual void open(const char* filename, std::ios::openmode mode)
      {
         FFTextStream::open(filename, mode);
         headerRead = false;
      }

         /// destructor; override to force 'close'
      virtual ~RinexUPDStream() { }


         /// true if the header has been read
      bool headerRead;             

   }; // class RinexUPDStream
   
   //@}
   
} // namespace gpstk

#endif // GPSTK_RINEX_UPD_STREAM_INCLUDE
