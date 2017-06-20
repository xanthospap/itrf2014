#include <fstream>
#include <cassert>
#include <string>
#include <exception>
#include <iostream>

float
read_ssc_header(const char* ssc_file, std::string& ref_frame)
{
    using pos_t =  std::string::size_type;

    constexpr pos_t max_chars {256};
    std::string line,
                middle_part{"STATION POSITIONS AT EPOCH"},
                last_part{"AND VELOCITIES"};
    line.reserve(max_chars);
    const char whitesp = ' ';

    std::ifstream fin (ssc_file);
    if ( !fin.is_open() ) {
        throw std::runtime_error
        ("ERROR. Failed to open file \""+std::string(ssc_file)+"\"");
    }
    
    std::getline(fin, line);

    // get the reference frame, which is the frst word in the line
    pos_t length = line.size();
    pos_t pos1   = line.find_first_not_of(whitesp);
    pos_t pos2   = line.find_first_of(whitesp, pos1);
    assert(pos2 > pos1);
    ref_frame = line.substr(pos1, pos2);
    std::cout<<"\n--frame ["<<ref_frame<<"]";

    // the header is actually pretty standard ....
    assert( length > pos2 + middle_part.size() );
    std::cout<<"\n--["<<line.substr(pos2, pos2+middle_part.size())<<"]";
    assert( line.substr(pos2, pos2+middle_part.size()) == middle_part );

    // get the reference epoch
    pos1 = pos2+middle_part.size();
    pos2 = line.find_first_of(whitesp, pos1+1);
    std::string ref_epoch {line.substr(pos1, pos2)};

    // check the last part
    assert( length >= pos2 + last_part.size() );
    assert( line.substr(pos2+1, pos2+last_part.size()) == last_part );

    // retun reference epoch as flost
    return std::stof(ref_epoch);
}

int main()
{
    const char* ssc_file = "ITRF2008_GNSS.SSC.txt";
    std::string reff;
    float       reft;
    reft = read_ssc_header(ssc_file, reff);

    std::cout<<"\nFrame is \""<<reff<<"\", time is "<<reft<<"\n";
    return 0;
}

/*
template<typename D>
    get_ssc_info(const char* ssc_file, const char* station,
        const ngpt::datetime<D>& t)
{
}
*/
