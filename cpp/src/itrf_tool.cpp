#include <cstring>
#include <map>
#include "itrf_tools.hpp"

int
parse_cmd(int argc, char* argv[], std::map<char, std::vector<std::string>>& cmd_map)
{
  std::cout<<"\nIn cmd parser (from main)";
  int dummy=0;
  // setup default positional arguments
  cmd_map['n'] = std::vector<std::string>{"0"}; //< psd only

  for (int i=1; i<argc; ) {
    if (++dummy>100) return 50;
    std::cout<<"\nIterating with "<<i<<"/"<<argc;
    if (!std::strcmp(argv[i], "-s") || !std::strcmp(argv[i], "--stations")) {
      std::cout<<"\nread option -s";
      if (argc<=i+1) return 1;
      ++i; cmd_map['s'] = std::vector<std::string>();
      while (i<argc) {
        if (argv[i][0]=='-') break;
        cmd_map['s'].emplace_back(std::string(argv[i]));
        ++i;
      }
    } else if (!std::strcmp(argv[i], "-m") || !std::strcmp(argv[i], "--domes")) {
      std::cout<<"\nread option -m";
      if (argc<=i+1) return 1;
      ++i; cmd_map['m'] = std::vector<std::string>();
      while (i<argc) {
        if (argv[i][0]=='-') break;
        cmd_map['m'].emplace_back(std::string(argv[i]));
        ++i;
      }
    } else if (!std::strcmp(argv[i], "-c") || !std::strcmp(argv[i], "--ssc")) {
      std::cout<<"\nread option -c";
      if (argc<=i+1) return 1;
      cmd_map['c'] = std::vector<std::string>{std::string(argv[i+1])};
      i+=2;
    } else if (!std::strcmp(argv[i], "-p") || !std::strcmp(argv[i], "--psd")) {
      std::cout<<"\nread option -p";
      if (argc<=i+1) return 1;
      cmd_map['p'] = std::vector<std::string>{std::string(argv[i+1])};
      i+=2;
    } else if (!std::strcmp(argv[i], "-y") || !std::strcmp(argv[i], "--year")) {
      std::cout<<"\nread option -y";
      if (argc<=i+1) return 1;
      cmd_map['y'] = std::vector<std::string>{std::string(argv[i+1])};
      i+=2;
    } else if (!std::strcmp(argv[i], "-d") || !std::strcmp(argv[i], "--doy")) {
      std::cout<<"\nread option -d";
      if (argc<=i+1) return 1;
      cmd_map['d'] = std::vector<std::string>{std::string(argv[i+1])};
      i+=2;
    } else if (!std::strcmp(argv[i], "--psd-only")) {
      std::cout<<"\nread option --psd-only";
      cmd_map['n'][0] = "1";
      i+=1;
    }
    else {
      std::cerr<<"\n[WARNING] Invalid command line argument \""<<argv[i]<<"\". Skipping";
      ++i;
    }
  }
  std::cout<<"\nRead cmd's";

  // make sure the user didn't mess up the args ...
  auto mend = cmd_map.end();
  auto i1=cmd_map.find('n');
  if (i1->second[0]=="1" && cmd_map.find('p')==mend) {
    std::cerr<<"\n[ERROR] If you need the PSD values, you need to supply a PSD file!";
    return 1;
  }
  if (i1->second[0]=="0" && cmd_map.find('c')==mend) {
    std::cerr<<"\n[ERROR] You need to supply an SSC file for coordinate extrapolation";
    return 1;
  }
  if (cmd_map.find('y')==mend || cmd_map.find('d')==mend) {
    std::cerr<<"\n[ERROR] Need to provide a year and a day_of_year";
    return 1;
  }
  std::cout<<"\nReturning to main";
  return 0;
}

int main(int argc, char* argv[])
{
  using mlsec = ngpt::milliseconds;
  
  std::cout<<"\nIn main; calling parse_cmd";
  std::map<char, std::vector<std::string>> cmd_map;
  if (parse_cmd(argc, argv, cmd_map)) return 10;
  std::cout<<"\nIn main again; returned from parser";

  auto mend = cmd_map.end();
  auto it=cmd_map.find('y'); int year=std::stoi(it->second[0]);
       it=cmd_map.find('d'); int doy =std::stoi(it->second[0]);
  ngpt::datetime<mlsec> t (ngpt::year{year}, ngpt::day_of_year{doy}, mlsec{0});
  std::vector<ngpt::sta_crd> results;

  // easy case: We have a PSD file but no SSC; Only compute PSD in [e,n,u]
  it=cmd_map.find('n');
  if (it->second[0]=="1") {
    int numsta1(0), numsta2(0);
    std::string psd_file = cmd_map['p'][0];
    if (auto its=cmd_map.find('s'); its!=mend) {
      numsta1 = compute_psd(psd_file.c_str(), its->second, t, results, false);
    }
    std::vector<ngpt::sta_crd> results_d;
    if (auto its=cmd_map.find('m'); its!=mend) {
      numsta2 = compute_psd(psd_file.c_str(), its->second, t, results_d, true);
    }
    results.insert(results.end(), std::make_move_iterator(results_d.begin()),
      std::make_move_iterator(results_d.end()));
    printf("\nNAME   DOMES         X(mm)          Y(mm)           Z(mm)        EPOCH");
    printf("\n---- --------- --------------- --------------- --------------- ------------------");
    for (const auto& i : results) {
      printf("\n%s %15.5f %15.5f %15.5f %s", i.site.c_str(), i.x, i.y, i.z, ngpt::strftime_ymd_hms(t).c_str());
    }
    return numsta1+numsta2-(int)results.size();
  }


  /*
  using mlsec = ngpt::milliseconds;

  const char* ssc_file = "ITRF2008_GNSS.SSC.txt";
  std::string reff;
  float       reft;

  std::ifstream fin (ssc_file);
  reft = ngpt::itrf_details::read_ssc_header(fin, reff);
  std::cout<<"\nFrame is \""<<reff<<"\", time is "<<reft<<"\n";

  int t0_yr = (int)reft;
  assert((float)t0_yr-reft == 0e0);
  ngpt::datetime<mlsec> t0 {ngpt::year{t0_yr}, ngpt::day_of_year{1}, mlsec{0}};

  std::vector<std::string> stations;
  stations.emplace_back("NRMD 92701M005");
  stations.emplace_back("COCO");
  stations.emplace_back("REUN 97401M003");
  stations.emplace_back("AZRY 49971M001");
  stations.emplace_back("ANKR");

  // ngpt::datetime<mlsec> t {ngpt::year{2017}, ngpt::day_of_year{143}, ngpt::hours{12},
  // ngpt::minutes{40}, mlsec{365554}};
  ngpt::datetime<mlsec> t{ngpt::year{2017}, ngpt::day_of_year{143}, mlsec{0}};

  std::vector<ngpt::sta_crd> sta_crd;

  ngpt::ssc_extrapolate(fin, stations, t, t0, sta_crd);
  printf("\nNAME   DOMES         X(m)           Y(m)            Z(m)        EPOCH");
  printf("\n---- --------- --------------- --------------- --------------- ------------------");
  for (const auto& i : sta_crd) {
    printf("\n%s %15.5f %15.5f %15.5f %s", i.site.c_str(), i.x, i.y, i.z, ngpt::strftime_ymd_hms(t).c_str());
  }

  ngpt::compute_psd("ITRF2014-psd-gnss.dat", stations, t, sta_crd);
  printf("\nNAME   DOMES         X(mm)          Y(mm)           Z(mm)        EPOCH");
  printf("\n---- --------- --------------- --------------- --------------- ------------------");
  for (const auto& i : sta_crd) {
    printf("\n%s %15.5f %15.5f %15.5f %s", i.site.c_str(), i.x, i.y, i.z, ngpt::strftime_ymd_hms(t).c_str());
  }

  std::cout<<"\n";
  return 0;
  */
}
