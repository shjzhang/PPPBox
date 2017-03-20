#ifndef NTRIPFILEBASE_HPP
#define NTRIPFILEBASE_HPP

#include <string>
#include "CommonTime.hpp"

class NtripFileBase
{
public:
    /// Constructor
    NtripFileBase();

    /// Destructor
    ~NtripFileBase() {;}

    /// Resolve file name according to specified standards
    void resolveFileName(gpstk::CommonTime& dateTime) {;}

    /// Print Header of the output File
    void printHeader() {;}

    /// Dump out the data by epoch
    void dumpEpoch() {;}

    /// Set the file path
    void setFilePath(std::string& path);

    /// Set the choice if write the rinex file
    void setWriteFile(bool flag)
    { m_bWriteFile = flag; }

    /// Get the file path
    std::string getFilePath()
    { return m_sFilePath;}
protected:
    std::string m_sFilePath;    ///< Path to save file
    std::string m_sFileName;    ///< File name

    bool m_bWriteFile;          ///< If write the data to file
    bool m_bHeaderWritten;      ///< If have written the file header
};

#endif // NTRIPFILEBASE_HPP
