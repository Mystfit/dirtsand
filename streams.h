/******************************************************************************
 * This file is part of dirtsand.                                             *
 *                                                                            *
 * dirtsand is free software: you can redistribute it and/or modify           *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 3 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * dirtsand is distributed in the hope that it will be useful,                *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with dirtsand.  If not, see <http://www.gnu.org/licenses/>.          *
 ******************************************************************************/

#ifndef _DS_STREAMS_H
#define _DS_STREAMS_H

#include "strings.h"

namespace DS
{
    class Stream
    {
    public:
        Stream() { }
        virtual ~Stream() { }

        virtual ssize_t readBytes(void* buffer, size_t count) = 0;
        virtual ssize_t writeBytes(const void* buffer, size_t count) = 0;

        template <typename tp> tp read()
        {
            tp value;
            readBytes(&value, sizeof(value));
            return value;
        }

        String readString(size_t length, DS::StringType format = e_StringRAW8);
        String readSafeString(DS::StringType format = e_StringRAW8);

        template <typename tp> void write(tp value)
        { writeBytes(&value, sizeof(value)); }

        void writeString(const String& value, DS::StringType format = e_StringRAW8);
        void writeSafeString(const String& value, DS::StringType format = e_StringRAW8);

        virtual uint64_t tell() = 0;
        virtual void seek(uint64_t offset, int whence) = 0;
        virtual uint64_t size() = 0;
        virtual bool ateof() = 0;
        virtual void flush() = 0;
    };

    class FileStream : public Stream
    {
        //
    };

    class BufferStream : public Stream
    {
        //
    };
}

#endif
