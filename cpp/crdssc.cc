#include <fstream>
#include <cassert>
#include <string>
#include <exception>
#include <iostream>
#include <functional>
#include <vector>
#include <algorithm>

#include "ggdatetime/dtcalendar.hpp"
#include "ggdatetime/datetime_write.hpp"

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
         line.compare(pos2+1, mdp_sz, middle_part) ) return -1e0;

    // get the reference epoch
    pos1 = pos2 + mdp_sz + 1;
    pos2 = line.find_first_of(whitesp, pos1+1);
    std::string ref_epoch {line.substr(pos1, pos2-pos1)};

    // check the last part
    if ( !(length >= pos2 + ltp_sz) ||
         line.compare(pos2+1, ltp_sz, last_part) ) return -1e0;

    // read a bunch of no-info lines ....
    for (int i = 0; i < 6; i++) std::getline(ssc_stream, line);

    // retun reference epoch as float
    return std::stof(ref_epoch);
}

template<typename S>
    struct ssc_record
{
    std::string site;        ///< NAME+' '+DOMES = 9+1+4 chars
    ngpt::datetime<S> from,  ///< Validity interval, from ...
                      to;    ///< Vlidity interval, to ...
    double x,y,z,            ///< Coordinates in m
           vx,vy,vz,         ///< Velocities in m/y
           sx,sy,sz,         ///< Coordinate sigmas
           svx,svy,svz;      ///< Velocity sigmas
};

template<typename S>
    int
    read_next_record(std::ifstream& ssc_stream, ssc_record<S>& record)
{
    constexpr int max_chars {256};
    std::string line;
    line.reserve(max_chars);
    
    // first line has domes, site_id, position info and validity interval
    if ( std::getline(ssc_stream, line) ) {
        record.site  = line.substr(32, 5);                 // 4-char id
        record.site += line.substr(0, 10);                 // domes
        std::size_t pos {36}, idx;
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
        record.from = ngpt::datetime<S>::min();     // preset from to min date
        record.to   = ngpt::datetime<S>::max();     // preset to to max date
        if ( (pos = line.find_first_of(':', pos)) != std::string::npos ) {
            int  iyr, idoy;
            long isec;
            // Resolve 'from' date string ...
            pos  -= 2;
            iyr   = std::stoi(line.substr(pos, 2), &idx);
            assert( idx == 2 );
            iyr  += (iyr > 70 ) ? (1900) : (2000);
            pos  += idx + 1;
            idoy  = std::stoi(line.substr(pos, 3), &idx);
            assert( idx == 3 );
            pos  += idx + 1;
            isec  = std::stol(line.substr(pos, 6), &idx); // seconds
            isec *= S::template sec_factor<long>(); // cast seconds to whatever S is
            if ( iyr + idoy + isec != 0 ) { 
                ngpt::datetime<S> tmp
                    {ngpt::year{iyr}, ngpt::day_of_year{idoy}, S{isec}};
                record.from = tmp;
            }
            // Resolve 'to' date sting ...
            pos  += idx;
            pos   = line.find_first_of(':', pos) - 2;
            iyr   = std::stoi(line.substr(pos, 2), &idx);
            iyr  += (iyr > 70 ) ? (1900) : (2000);
            pos  += idx + 1;
            idoy  = std::stoi(line.substr(pos, 3), &idx);
            assert( idx == 3 );
            pos  += idx + 1;
            isec  = std::stol(line.substr(pos));
            isec *= S::template sec_factor<long>();
            if ( iyr + idoy + isec != 0 ) { 
                ngpt::datetime<S> tmp
                    {ngpt::year{iyr}, ngpt::day_of_year{idoy}, S{isec}};
                record.to = tmp;
            }
        }
    } else {
        return 1;
    }
    
    // second line has velocity info
    if ( std::getline(ssc_stream, line) ) {
        assert( !line.compare(0, 9, record.site, 5, 9 ) );
        std::size_t pos {36}, idx;
        record.vx = std::stod(line.substr(pos, 20), &idx);   // vx
        pos += idx;
        record.vy = std::stod(line.substr(pos, 20), &idx);   // vy
        pos += idx;
        record.vz = std::stod(line.substr(pos, 20), &idx);   // vz
        pos += idx;
        record.svx = std::stod(line.substr(pos, 20), &idx);  // svx
        pos += idx;
        record.svy = std::stod(line.substr(pos, 20), &idx);  // svy
        pos += idx;
        record.svz = std::stod(line.substr(pos, 20), &idx);  // svz
    } else {
        return 1;
    }
    
    return 0;
}

int
compare_sta_id(const std::string& str1, const std::string& str2)
{ return str1.compare(0, 4, str2, 0, 4); }
int
compare_sta_domes(const std::string& str1, const std::string& str2)
{ return str1.compare(5, 9, str2, 5, 9); }

template<typename S>
    int
    ssc_extrapolate(std::ifstream& fin, const std::vector<std::string>& stations, 
        const ngpt::datetime<S>& t, const ngpt::datetime<S>& t0, 
        std::vector<std::tuple<std::string, double, double, double>>& results,
        bool use_domes = false)
{
    std::function<int(const std::string&, const std::string&)> cmp
        = compare_sta_id;
    if ( use_domes ) cmp = compare_sta_domes;
    ngpt::datetime_interval dt {ngpt::delta_date(t, t0)};
    double dyr = dt.as_mjd() / 365.25;

    ssc_record<S> record;
    std::vector<std::string> sta {stations};
    auto it = sta.begin();
    std::string site;
    std::cout.setf(std::ios::fixed);
    std::cout.precision(5);
    while ( !read_next_record<S>(fin, record) && sta.size() ) {
        site = record.site;
        if ( (it = std::find_if(sta.begin(), sta.end(),
            [=](const std::string& str)
                {return !cmp(site, str);})) != sta.end() ) {
            if ( t >= record.from && t < record.to ) {
                sta.erase(it);
                auto x = record.x + (record.vx * dyr);
                auto y = record.y + (record.vy * dyr);
                auto z = record.z + (record.vz * dyr);
                results.emplace_back( {site, x, y, z} );
            }
        }
    }
    return sta.size();
}

int main()
{
    using mlsec = ngpt::milliseconds;

    const char* ssc_file = "ITRF2008_GNSS.SSC.txt";
    std::string reff;
    float       reft;

    std::ifstream fin (ssc_file);
    reft = read_ssc_header(fin, reff);
    std::cout<<"\nFrame is \""<<reff<<"\", time is "<<reft<<"\n";

    std::vector<std::string> stations;
    stations.emplace_back("NRMD 92701M005");
    stations.emplace_back("REUN 97401M003");
    stations.emplace_back("AZRY 49971M001");

    ngpt::datetime<mlsec> t {ngpt::year{2017}, ngpt::day_of_year{143}, ngpt::hours{12},
    ngpt::minutes{40}, mlsec{365554}};
    ssc_extrapolate(fin, stations, t);

    return 0;
}
