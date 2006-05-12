/***********************************************************************
    filename:   CEGUIDefaultLogger.cpp
    created:    25/1/2006
    author:     Andrew Zabolotny
    
    purpose:    Implementation of the DefaultLogger class
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2006 Paul D Turner & The CEGUI Development Team
 *
 *   Permission is hereby granted, free of charge, to any person obtaining
 *   a copy of this software and associated documentation files (the
 *   "Software"), to deal in the Software without restriction, including
 *   without limitation the rights to use, copy, modify, merge, publish,
 *   distribute, sublicense, and/or sell copies of the Software, and to
 *   permit persons to whom the Software is furnished to do so, subject to
 *   the following conditions:
 *
 *   The above copyright notice and this permission notice shall be
 *   included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *   OTHER DEALINGS IN THE SOFTWARE.
 ***************************************************************************/
#include "CEGUIDefaultLogger.h"
#include <ctime>
#include <iomanip>

// Start of CEGUI namespace section
namespace CEGUI
{

    /*************************************************************************
        Constructor
    *************************************************************************/
    DefaultLogger::DefaultLogger(void) :
            d_caching(true)
    {
        // create log header
        logEvent("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+");
        logEvent("+                     Crazy Eddie's GUI System - Event log                    +");
        logEvent("+                          (http://www.cegui.org.uk/)                         +");
        logEvent("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
        logEvent("CEGUI::Logger singleton created.");
    }

    /*************************************************************************
        Destructor
    *************************************************************************/
    DefaultLogger::~DefaultLogger(void)
    {
        if (d_ostream.is_open())
        {
            logEvent("CEGUI::Logger singleton destroyed.");
            d_ostream.close();
        }
    }

    /*************************************************************************
        Logs an event
    *************************************************************************/
    void DefaultLogger::logEvent(const String& message, LoggingLevel level /* = Standard */)
    {
        using namespace std;

        time_t  et;
        time(&et);
        tm* etm = localtime(&et);

        if (etm)
        {
            // clear sting stream
            d_workstream.str("");

            // write date
            d_workstream << setfill('0') << setw(2) << etm->tm_mday << '/' <<
            setfill('0') << setw(2) << 1 + etm->tm_mon << '/' <<
            setw(4) << (1900 + etm->tm_year) << ' ';

            // wite time
            d_workstream << setfill('0') << setw(2) << etm->tm_hour << ':' <<
            setfill('0') << setw(2) << etm->tm_min << ':' <<
            setfill('0') << setw(2) << etm->tm_sec << ' ';

            // write event type code
            switch(level)
            {
            case Errors:
                d_workstream << "(Error)\t";
                break;

            case Standard:
                d_workstream << "(InfL1)\t";
                break;

            case Informative:
                d_workstream << "(InfL2)\t";
                break;

            case Insane:
                d_workstream << "(InfL3)\t";
                break;

            default:
                d_workstream << "(Unkwn)\t";
                break;
            }

            d_workstream << message << endl;

            if (d_caching)
            {
                d_cache.push_back(std::make_pair(d_workstream.str(), level));
            }
            else if (d_level >= level)
            {
                // write message
                d_ostream << d_workstream.str();
                // ensure new event is written to the file, rather than just being buffered.
                d_ostream.flush();
            }
        }
    }

    void DefaultLogger::setLogFilename(const String& filename, bool append)
    {
        // close current log file (if any)
        if (d_ostream.is_open())
        {
            d_ostream.close();
        }

        d_ostream.open(filename.c_str(), std::ios_base::out | (append ? std::ios_base::app : std::ios_base::trunc));

        if (!d_ostream)
        {
            throw "Logger::setLogFilename - Failed to open file.";
        }
        
        // initialise width for date & time alignment.
        d_ostream.width(2);

        // write out cahced log strings.
        if (d_caching)
        {
            d_caching = false;

            std::vector<std::pair<String, LoggingLevel> >::iterator iter = d_cache.begin();

            while (iter != d_cache.end())
            {
                if (d_level >= (*iter).second)
                {
                    // write message
                    d_ostream << (*iter).first;
                    // ensure new event is written to the file, rather than just being buffered.
                    d_ostream.flush();
                }

                ++iter;
            }

            d_cache.clear();
        }
    }

} // End of  CEGUI namespace section
