#include "itrf_tools.hpp"

int main()
{
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
}
