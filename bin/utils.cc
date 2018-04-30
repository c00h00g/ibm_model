#include <string>
#include <vector>

#include "utils.h"

void split_string(const std::string & input,
                  std::vector<std::string> & output,
                  const std::string & separator) {
    std::string::size_type begin = 0;
    std::string::size_type end = input.find(separator);
    while (std::string::npos != end) {
            output.push_back(input.substr(begin, end - begin));
            begin = end + separator.length();
            end = input.find(separator, begin);
    }
    if (begin != input.length()) {
            output.push_back(input.substr(begin));
    }
}
