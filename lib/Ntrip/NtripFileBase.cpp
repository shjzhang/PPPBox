/**
  * @file NtripFileBase.cpp
  * Base class to solve file problem of decoded data.
  */

#include "NtripFileBase.hpp"
#include "FileSpec.hpp"
#include "FileUtils.hpp"

NtripFileBase::NtripFileBase()
{
    m_sFilePath = ".";
    m_bWriteFile = false;
    m_bHeaderWritten = false;
    setFilePath(m_sFilePath);
}


void NtripFileBase::setFilePath(std::string &path)
{
    if(!path.empty())
    {
        if(path[path.size()-1]!=slash)
        {
            path += slash;
        }
        m_sFilePath = path;
    }
    else
    {
        m_sFilePath = "." + slash;
    }
    // make the directory in case that the file path does not exit.
    unsigned mode = 0755;
    gpstk::FileUtils::makeDir(m_sFilePath.c_str(),mode);
}
