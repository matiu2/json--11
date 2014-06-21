%%{
    machine string;

    action recordBackspace { output += '\b'; }
    action recordFormFeed { output += '\f'; }
    action recordNewLine { output += '\n'; }
    action recordReturn { output += '\r'; }
    action recordTab { output += '\t'; }
    action getChar { output += *p; }
    action startUnicode {
        uniChar = 0;
        uniCharBytes = 0;
    }
    action getUnicode {
        if (uniCharBytes++ == 4)
            throw std::logic_error("Max unicode char is 32 bits");
        uniChar <<= 4;
        char ch = *p;
        if (ch >= 'a') 
            uniChar += ch - 'a' + 0x0A;
        else if (ch >= 'A')
            uniChar += ch - 'A' + 0x0A;
        else
            uniChar += ch - '0';
    }
    action endUnicode {
        /*
           UCS-4 range (hex.)           UTF-8 octet sequence (binary)
           0000 0000-0000 007F   0xxxxxxx
           0000 0080-0000 07FF   110xxxxx 10xxxxxx
           0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx

           0001 0000-001F FFFF   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
           0020 0000-03FF FFFF   111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
           0400 0000-7FFF FFFF   1111110x 10xxxxxx ... 10xxxxxx
        */
        int numBytes = getNumBytes(uniChar); // number of bytes needed for utf-8 encoding
        if (numBytes == 1) {
            output += uniChar;
        } else {
            std::vector<char> bytes;
            for (int i=1; i<numBytes; ++i) {
                char byte = uniChar & 0x3f; // Only encoding 6 bits right now
                byte |= 0x80; // Make sure the high bit is set
                bytes.push_back(byte);
                uniChar >>= 6;
            }
            // The last byte is special
            char mask = 0x3f >> (numBytes - 2);
            char byte = uniChar & mask;
            char top = 0xc0;
            for (int i=2; i<numBytes; ++i) {
                top >>= 1;
                top |= 0x80;
            }
            byte |= top;
            bytes.push_back(byte);
            // Output it
            for (auto i=bytes.rbegin(); i!=bytes.rend(); ++i)
                output += *i;
        }
    }
    action gotString {
        ++p;
        return output;
    }

    esc_b = "\\b"@recordBackspace;
    esc_f = "\\f"@recordFormFeed;
    esc_n = "\\n"@recordNewLine;
    esc_r = "\\r"@recordReturn;
    esc_t = "\\t"@recordTab;
    normal_char = [^\\"]@getChar;
    hex_digit = [0-9a-fA-F]@getUnicode;
    esc_uni = '\\u'@startUnicode.hex_digit+ % endUnicode;
    esc_any = "\\".[^bfnrut]@getChar;

    string = (esc_b|esc_f|esc_n|esc_r|esc_t|esc_any|esc_uni|normal_char)**:>'"' @ gotString; 
}%%
