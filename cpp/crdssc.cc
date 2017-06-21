#include <fstream>
#include <cassert>
#include <string>
#include <exception>
#include <iostream>

/// Function to read the header off from a SSC-type file.
///
/// Given an SSC-type input file stream (ascii), this function will read the
/// first line and extract information (i.e. reference frame name and 
/// reference epoch). Then it will read (and skip) the next 6 lines, so that
/// the input stream is at a position of reading record lines (i.e. next line
/// to be read from the stream will be the first record line).
/// The function will automatically go to the top of the file to read the
/// header.
///
/// @param[in] ssc_stream An SSC-type input file stream (aka std::ifstream)
/// @param[out] ref_frame The name of the reference frame as extracted from 
///                       the file header (atually the first word of the first
///                       line).
/// @return               The reference epoch as float, i.e. as fractional year.
///                       or -1 to signal that somethin went wrong while
///                       resolving the fields of the first line.
/// @note  Always check that the returned year is greater than 0. If not, then
///        the header was not read properly.
///
float
read_ssc_header(std::ifstream& ssc_stream, std::string& ref_frame)
{
    using pos_t =  std::string::size_type;

    constexpr pos_t max_chars {256};
    const     std::string middle_part {"STATION POSITIONS AT EPOCH"},
                          last_part   {"AND VELOCITIES"};
    const     pos_t mdp_sz {middle_part.size()},
                    ltp_sz {last_part.size()};
    const char whitesp = ' ';
    auto npos = std::string::npos;
    std::string line;
    line.reserve(max_chars);

    ssc_stream.seekg(0, std::ios::beg);
    std::getline(ssc_stream, line);

    // get the reference frame, which is the frst word in the line
    pos_t length = line.size();
    pos_t pos1   = line.find_first_not_of(whitesp);
    pos_t pos2   = line.find_first_of(whitesp, pos1);
    if ( !((pos1 != npos && pos2 != npos) && (pos2 > pos1)) ) return -1e0;
    ref_frame    = line.substr(pos1, pos2-pos1);

    // the header is actually pretty standard .... check the middle part
    if ( !(length > pos2 + mdp_sz) ||
         !(line.substr(pos2+1, mdp_sz) == middle_part) ) return -1e0;

    // get the reference epoch
    pos1 = pos2 + mdp_sz + 1;
    pos2 = line.find_first_of(whitesp, pos1+1);
    std::string ref_epoch {line.substr(pos1, pos2-pos1)};

    // check the last part
    if ( !(length >= pos2 + ltp_sz) ||
         !(line.substr(pos2+1, ltp_sz) == last_part) ) return -1e0;

    // read a bunch of no-info lines ....
    for (int i = 0; i < 6; i++) std::getline(ssc_stream, line);

    // retun reference epoch as float
    return std::stof(ref_epoch);
}

template<typename S>
    struct ssc_record
{
    std::string site;        ///< DOMES+' '+NAME = 9+1+4 chars
    ngpt::datetime<S> from,  ///< Validity interval, from ...
                      to;    ///< Vlidity interval, to ...
    double x,y,z,            ///< Coordinates in m
           vx,vy,vz,         ///< Velocities in m/y
           sx,sy,sz,         ///< Coordinate sigmas
           svx,svy,svz;      ///< Velocity sigmas
    void initialize()
    {
        site.reserve(15);
        from = ngpt::datetime<S>::min();
        to   = ngpt::datetime<S>::max();
        return;
    }
}

template<typename S>
    int
    read_next_record(std::ifstream& ssc_stream, ssc_record& record)
{
    constexpr int max_chars {256};
    std::string line;
    line.reserve(max_chars);
    
    if ( std::getline(ssc_stream, line) ) {
        record.site  = line.substr(0, 10);                 // domes
        record.size += line.substr(32, 4);                 // 4-char id
        std::size_t pos {32}, idx;
        record.x = std::stod(line.substr(pos, 20), &idx);   // x
        pos += idx;
        record.y = std::stod(line.substr(pos, 20), &idx);   // y
        pos += idx;
        record.z = std::stod(line.substr(pos, 20), &idx);   // z
        pos += idx;
        record.sx = std::stod(line.substr(pos, 20), &idx);  // sx
        pos += idx;
        record.sy = std::stod(line.substr(pos, 20), &idx);  // sy
        pos += idx;
        record.sz = std::stod(line.substr(pos, 20), &idx);  // sz
        pos += idx;
        if ( (pos = line.find_first_of(':', pos)) != std::string::npos ) {

        }
    }
}

int main()
{
    const char* ssc_file = "ITRF2008_GNSS.SSC.txt";
    std::string reff;
    float       reft;

    std::ifstream fin (ssc_file);
    reft = read_ssc_header(fin, reff);

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
