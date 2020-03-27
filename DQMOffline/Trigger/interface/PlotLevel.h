#ifndef __PlotLevel__
#define __PlotLevel__

#include "FWCore/ParameterSet/interface/ParameterSet.h"

//#include "DQMOffline/Trigger/interface/HLTTauDQMPlotter.h"
#include "DQMServices/Core/interface/MonitorElement.h"
//typedef dqm::reco::MonitorElement MonitorElement;
typedef dqm::legacy::MonitorElement MonitorElement;

enum PL {kEverything, kVital};

class PlotLevel {
 public:
  PlotLevel(const edm::ParameterSet&);
  ~PlotLevel();

  MonitorElement* book(MonitorElement* me, PL level=kEverything);
  bool setLevel(PL level);
  
 private:
  PL plotlevel;
  int cAllHistograms;
  int cPlottedHistograms;
};
#endif
