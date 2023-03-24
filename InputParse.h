#ifndef __INPUT_PARSER__
#define __INPUT_PARSER__

#include <vector>
#include <string>
#include <algorithm>

class InputParser{
    private:
        std::vector <std::string> tokens;
    
    public:
        InputParser (int &argc, char **argv);

        /// @author iain
        const std::string& getCmdOption(const std::string &option) const;

        /// @author iain
        bool cmdOptionExists(const std::string &option) const;
};

#endif