#include "DQMOffline/Trigger/interface/PlotLevel.h"

PlotLevel::PlotLevel(const edm::ParameterSet& pset) {
  plotlevel = (PL)pset.getUntrackedParameter<int>("PlotLevel", int(kEverything));
  cAllHistograms = 0;
  cPlottedHistograms = 0;
}
PlotLevel::~PlotLevel(){
  std::string s_pl = "kEverything";
  if(plotlevel == 1) s_pl = "kVital";

  std::cout << "Plot level " << plotlevel << " " << s_pl << std::endl;
  std::cout << "Plotting " << cPlottedHistograms << " out of " << cAllHistograms << std::endl;
}

MonitorElement* PlotLevel::book(MonitorElement* me, PL level){
  cAllHistograms++;
  if(level>=plotlevel){
    cPlottedHistograms++;
    return me;
  }
  return 0;
}

bool PlotLevel::setLevel(PL level){
  cAllHistograms++;
  if(level>=plotlevel){
    cPlottedHistograms++;
    return true;
  }
  return false;
}
