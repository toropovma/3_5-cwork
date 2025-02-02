#include "data.h"

std::ostream& operator<<(std::ostream& ostream, const Data& data)
{
    ostream << "{\n"
            << "  original[" << data.original.size() << "] = [" << data.original << "]\n"
            << "  sorted[" << data.sorted.size() << "] = [" << data.sorted << "]\n"
            << "}"; 
    return ostream;
}
