/*

SeriaLink 0.1.0

SeriaLink

Copyright (C) 2017 by Xose Pérez <xose dot perez at gmail dot com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef SerialLink_h
#define SerialLink_h

#include <Arduino.h>

#define DEFAULT_TERMINATE_CHAR  '\n'
#define DEFAULT_SPLIT_CHAR      '='
#define DEFAULT_QUERY_CHAR      '?'
#define MAX_BUFFER_SIZE         32

class SerialLink {

    public:

        SerialLink(Stream& serial,
            bool doACK = true,
            char splitChar = DEFAULT_SPLIT_CHAR,
            char queryChar = DEFAULT_QUERY_CHAR,
            char terminateChar = DEFAULT_TERMINATE_CHAR);

        void handle();
        void onGet(bool (*callback)(char * command));
        void onSet(bool (*callback)(char * command, long payload));
        void clear();

        void sendRaw(const char * string);
        void sendRaw_P(const char * string);
        bool send(const char * command, long payload);
        bool send(const char * command, long payload, bool doACK);
        bool send_P(const char * command, long payload);
        bool send_P(const char * command, long payload, bool doACK);
        void sendOK();
        void sendInvalid();

        void query(const char * command);
        void query_P(const char * command);

    private:

        Stream * _serial;
        bool _doACK = true;
        char _terminateChar = DEFAULT_TERMINATE_CHAR;
        char _splitChar = DEFAULT_SPLIT_CHAR;
        char _queryChar = DEFAULT_QUERY_CHAR;
        bool (*_onGet)(char *) = NULL;
        bool (*_onSet)(char *, long) = NULL;

};

#endif
