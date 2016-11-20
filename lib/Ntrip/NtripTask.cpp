#include <mutex>
#include  <fstream>
#include "NtripTask.h"
#include "NetUrl.hpp"
#include "NetQueryBase.hpp"
#include "NetQueryNtrip1.hpp"
//#include "MountPoint.hpp"


//using namespace gpstk;

NtripTask::NtripTask()
{
    m_bOutputRaw = false;
    m_sRawOutFile = "";
}

NtripTask::~NtripTask()
{

}

void NtripTask::run()
{
	string tempVersion = m_MP.getNtripVersion();
	int tempNmeaFlag = m_MP.getnmeaFlag();
	NetUrl tempURL = m_MP.getMountPointUrl();

	/// According to the ntrip version of mountpoint,
	/// select corresponding model of network request.
	/// By default: Ntrip Version 1.0
	if(tempVersion == "U")
	{}
	else if(tempVersion == "R")
	{}
	else if(tempVersion == "S")
	{}
	else if(tempVersion == "N")
	{}
	else if(tempVersion == "UN")
	{}
	else if(tempVersion == "2")
	{}
	else if(tempVersion == "2s")
	{}
    else if(tempVersion == "1")
	{
		/// 
		NetQueryBase* query = new NetQueryNtrip1();

        ofstream out(m_sRawOutFile.c_str(),ios::out|ios::binary);
        cout << "pid=" << this_thread::get_id() << endl;
        while(1)
        {
            if(tempNmeaFlag)
            {
                // added gga into string
                string gga = "";
                query->startRequest(tempURL,gga);
            }
            else
            {
                query->startRequest(tempURL,"");
                if(m_bOutputRaw)
                {
                    query->writeRawData(out);
                }
            }
        }
        out.close();
	}
}

