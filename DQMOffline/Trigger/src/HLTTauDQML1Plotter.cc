#include "DQMOffline/Trigger/interface/HLTTauDQML1Plotter.h"

#include "FWCore/Framework/interface/Event.h"

#include <cstring>

namespace {
  double getMaxEta(int binsEta, double widthEta) {
    if (widthEta <= 0.0) {
      edm::LogWarning("HLTTauDQMOffline") << "HLTTauDQML1Plotter::HLTTauDQML1Plotter: EtaHistoBinWidth = " << widthEta
                                          << " <= 0, using default value 0.348 instead";
      widthEta = 0.348;
    }
    return binsEta / 2 * widthEta;
  }
}  // namespace

HLTTauDQML1Plotter::HLTTauDQML1Plotter(const edm::ParameterSet& ps,
                                       edm::ConsumesCollector&& cc,
                                       int phibins,
                                       double maxpt,
                                       double maxhighpt,
                                       bool ref,
                                       double dr,
                                       const std::string& dqmBaseFolder)
    : HLTTauDQMPlotter(ps, dqmBaseFolder),
      doRefAnalysis_(ref),
      matchDeltaR_(dr),
      maxPt_(maxpt),
      maxHighPt_(maxhighpt),
      binsEt_(ps.getUntrackedParameter<int>("EtHistoBins", 25)),
      binsEta_(ps.getUntrackedParameter<int>("EtaHistoBins", 14)),
      binsPhi_(phibins),
      maxEta_(getMaxEta(binsEta_, ps.getUntrackedParameter<double>("EtaHistoBinWidth", 0.348))) {
  if (!configValid_)
    return;

  //Process PSet
  l1stage2Taus_ = ps.getUntrackedParameter<edm::InputTag>("L1Taus");
  l1stage2TausToken_ = cc.consumes<l1t::TauBxCollection>(l1stage2Taus_);

  l1stage2Sums_ = ps.getUntrackedParameter<edm::InputTag>("L1ETM");
  l1stage2SumsToken_ = cc.consumes<l1t::EtSumBxCollection>(l1stage2Sums_);
  l1ETMMin_ = ps.getUntrackedParameter<double>("L1ETMMin");

  configValid_ = true;
}

void HLTTauDQML1Plotter::bookHistograms(DQMStore::IBooker& iBooker, PlotLevel* pl) {
  if (!configValid_)
    return;

  // The L1 phi plot is asymmetric around 0 because of the discrete nature of L1 phi
  constexpr float pi = 3.1416f;
  constexpr float phiShift = pi / 18;  // half of 2pi/18 bin
  constexpr float minPhi = -pi + phiShift;
  constexpr float maxPhi = pi + phiShift;

  constexpr int BUFMAX = 256;
  char buffer[BUFMAX] = "";

  //Create the histograms
  iBooker.setCurrentFolder(triggerTag());

  l1tauEt_ = pl->book(iBooker.book1D("L1TauEt", "L1 #tau E_{T};L1 #tau E_{T};entries", binsEt_, 0, maxPt_),kEverything);
  l1tauEta_ = pl->book(iBooker.book1D("L1TauEta", "L1 #tau #eta;L1 #tau #eta;entries", binsEta_, -maxEta_, maxEta_),kEverything);
  l1tauPhi_ = pl->book(iBooker.book1D("L1TauPhi", "L1 #tau #phi;L1 #tau #phi;entries", binsPhi_, minPhi, maxPhi),kEverything);

  l1isotauEt_ = pl->book(iBooker.book1D("L1IsoTauEt", "L1 isolated #tau E_{T};L1 #tau E_{T};entries", binsEt_, 0, maxPt_),kEverything);
  l1isotauEta_ =
    pl->book(iBooker.book1D("L1IsoTauEta", "L1 isolated #tau #eta;L1 #tau #eta;entries", binsEta_, -maxEta_, maxEta_),kEverything);
  l1isotauPhi_ = pl->book(iBooker.book1D("L1IsoTauPhi", "L1 isolated #tau #phi;L1 #tau #phi;entries", binsPhi_, minPhi, maxPhi),kEverything);

  l1etmEt_ = pl->book(iBooker.book1D("L1ETM", "L1 ETM E_{T};L1 ETM E_{T};entries", binsEt_, 0, maxPt_),kEverything);
  l1etmPhi_ = pl->book(iBooker.book1D("L1ETMPhi", "L1 ETM #phi;L1 ETM #phi;entries", binsPhi_, minPhi, maxPhi),kEverything);

  snprintf(buffer, BUFMAX, "L1 leading #tau E_{T};L1 #tau E_{T};entries");
  firstTauEt_ = pl->book(iBooker.book1D("L1LeadTauEt", buffer, binsEt_, 0, maxPt_),kEverything);
  snprintf(buffer, BUFMAX, "L1 leading #tau #eta;L1 #tau #eta;entries");
  firstTauEta_ = pl->book(iBooker.book1D("L1LeadTauEta", buffer, binsEta_, -maxEta_, maxEta_),kEverything);
  snprintf(buffer, BUFMAX, "L1 leading #tau #phi;L1 #tau #phi;entries");
  firstTauPhi_ = pl->book(iBooker.book1D("L1LeadTauPhi", buffer, binsPhi_, minPhi, maxPhi),kEverything);

  snprintf(buffer, BUFMAX, "L1 second-leading #tau E_{T};L1 #tau E_{T};entries");
  secondTauEt_ = pl->book(iBooker.book1D("L1SecondTauEt", buffer, binsEt_, 0, maxPt_),kEverything);
  snprintf(buffer, BUFMAX, "L1 second-leading #tau #eta;L1 #tau #eta;entries");
  secondTauEta_ = pl->book(iBooker.book1D("L1SecondTauEta", buffer, binsEta_, -maxEta_, maxEta_),kEverything);
  snprintf(buffer, BUFMAX, "L1 second-leading #tau #phi;L1 #tau #phi;entries");
  secondTauPhi_ = pl->book(iBooker.book1D("L1SecondTauPhi", buffer, binsPhi_, minPhi, maxPhi),kEverything);

  snprintf(buffer, BUFMAX, "L1 leading isolated #tau E_{T};L1 #tau E_{T};entries");
  firstIsoTauEt_ = pl->book(iBooker.book1D("L1LeadIsoTauEt", buffer, binsEt_, 0, maxPt_),kEverything);
  snprintf(buffer, BUFMAX, "L1 leading isolated #tau #eta;L1 #tau #eta;entries");
  firstIsoTauEta_ = pl->book(iBooker.book1D("L1LeadIsoTauEta", buffer, binsEta_, -maxEta_, maxEta_),kEverything);
  snprintf(buffer, BUFMAX, "L1 leading isolated #tau #phi;L1 #tau #phi;entries");
  firstIsoTauPhi_ = pl->book(iBooker.book1D("L1LeadIsoTauPhi", buffer, binsPhi_, minPhi, maxPhi),kEverything);

  snprintf(buffer, BUFMAX, "L1 second-leading isolated #tau E_{T};L1 #tau E_{T};entries");
  secondIsoTauEt_ = pl->book(iBooker.book1D("L1SecondIsoTauEt", buffer, binsEt_, 0, maxPt_),kEverything);
  snprintf(buffer, BUFMAX, "L1 second-leading isolated #tau #eta;L1 #tau #eta;entries");
  secondIsoTauEta_ = pl->book(iBooker.book1D("L1SecondIsoTauEta", buffer, binsEta_, -maxEta_, maxEta_),kEverything);
  snprintf(buffer, BUFMAX, "L1 second-leading isolated #tau #phi;L1 #tau #phi;entries");
  secondIsoTauPhi_ = pl->book(iBooker.book1D("L1SecondIsoTauPhi", buffer, binsPhi_, minPhi, maxPhi),kEverything);

  if (doRefAnalysis_) {
    l1tauEtRes_ = pl->book(iBooker.book1D(
        "L1TauEtResol", "L1 #tau E_{T} resolution;[L1 #tau E_{T}-Ref #tau E_{T}]/Ref #tau E_{T};entries", 60, -1, 4),kEverything);
    l1isotauEtRes_ = pl->book(iBooker.book1D("L1IsoTauEtResol",
                       "L1 isolated #tau E_{T} resolution;[L1 #tau E_{T}-Ref #tau E_{T}]/Ref #tau E_{T};entries",
                       60,
                       -1,
                       4),kEverything);

    iBooker.setCurrentFolder(triggerTag() + "/helpers");

    l1tauEtEffNum_ =
      pl->book(iBooker.book1D("L1TauEtEffNum", "L1 #tau E_{T} Efficiency;Ref #tau E_{T};entries", binsEt_, 0, maxPt_),kEverything);
    l1tauHighEtEffNum_ = pl->book(iBooker.book1D(
		 "L1TauHighEtEffNum", "L1 #tau E_{T} Efficiency (high E_{T});Ref #tau E_{T};entries", binsEt_, 0, maxHighPt_),kEverything);

    l1tauEtEffDenom_ =
      pl->book(iBooker.book1D("L1TauEtEffDenom", "L1 #tau E_{T} Denominator;Ref #tau E_{T};entries", binsEt_, 0, maxPt_),kEverything);
    l1tauHighEtEffDenom_ = pl->book(iBooker.book1D("L1TauHighEtEffDenom",
                                          "L1 #tau E_{T} Denominator (high E_{T});Ref #tau E_{T};Efficiency",
                                          binsEt_,
                                          0,
					  maxHighPt_),kEverything);

    l1tauEtaEffNum_ =
      pl->book(iBooker.book1D("L1TauEtaEffNum", "L1 #tau #eta Efficiency;Ref #tau #eta;entries", binsEta_, -maxEta_, maxEta_),kEverything);
    l1tauEtaEffDenom_ = pl->book(iBooker.book1D(
	"L1TauEtaEffDenom", "L1 #tau #eta Denominator;Ref #tau #eta;entries", binsEta_, -maxEta_, maxEta_),kEverything);

    l1tauPhiEffNum_ =
      pl->book(iBooker.book1D("L1TauPhiEffNum", "L1 #tau #phi Efficiency;Ref #tau #phi;entries", binsPhi_, minPhi, maxPhi),kEverything);
    l1tauPhiEffDenom_ = pl->book(iBooker.book1D(
	"L1TauPhiEffDenom", "L1 #tau #phi Denominator;Ref #tau #phi;Efficiency", binsPhi_, minPhi, maxPhi),kEverything);

    l1isotauEtEffNum_ = pl->book(iBooker.book1D(
	"L1IsoTauEtEffNum", "L1 isolated #tau E_{T} Efficiency;Ref #tau E_{T};entries", binsEt_, 0, maxPt_),kEverything);
    l1isotauEtEffDenom_ = pl->book(iBooker.book1D(
        "L1IsoTauEtEffDenom", "L1 isolated #tau E_{T} Denominator;Ref #tau E_{T};entries", binsEt_, 0, maxPt_),kEverything);

    l1isotauEtaEffNum_ = pl->book(iBooker.book1D(
        "L1IsoTauEtaEffNum", "L1 isolated #tau #eta Efficiency;Ref #tau #eta;entries", binsEta_, -maxEta_, maxEta_),kEverything);
    l1isotauEtaEffDenom_ = pl->book(iBooker.book1D(
        "L1IsoTauEtaEffDenom", "L1 isolated #tau #eta Denominator;Ref #tau #eta;entries", binsEta_, -maxEta_, maxEta_),kEverything);

    l1isotauPhiEffNum_ = pl->book(iBooker.book1D(
        "L1IsoTauPhiEffNum", "L1 isolated #tau #phi Efficiency;Ref #tau #phi;entries", binsPhi_, minPhi, maxPhi),kEverything);
    l1isotauPhiEffDenom_ = pl->book(iBooker.book1D(
        "L1IsoTauPhiEffDenom", "L1 isolated #tau #phi Denominator;Ref #tau #phi;Efficiency", binsPhi_, minPhi, maxPhi),kEverything);

    l1etmEtEffNum_ = pl->book(iBooker.book1D("L1ETMEtEffNum", "L1 ETM Efficiency;Ref MET;entries", binsEt_, 0, maxPt_),kEverything);
    l1etmEtEffDenom_ = pl->book(iBooker.book1D("L1ETMEtEffDenom", "L1 ETM Denominator;Ref MET;entries", binsEt_, 0, maxPt_),kEverything);
  }
}

HLTTauDQML1Plotter::~HLTTauDQML1Plotter() = default;

//
// member functions
//

void HLTTauDQML1Plotter::analyze(const edm::Event& iEvent,
                                 const edm::EventSetup& iSetup,
                                 const HLTTauDQMOfflineObjects& refC) {
  if (doRefAnalysis_) {
    //Tau reference
    for (auto const& tau : refC.taus) {
      if(l1tauEtEffDenom_) l1tauEtEffDenom_->Fill(tau.pt());
      if(l1tauHighEtEffDenom_) l1tauHighEtEffDenom_->Fill(tau.pt());

      if(l1tauEtaEffDenom_) l1tauEtaEffDenom_->Fill(tau.eta());

      if(l1tauPhiEffDenom_) l1tauPhiEffDenom_->Fill(tau.phi());

      if(l1isotauEtEffDenom_) l1isotauEtEffDenom_->Fill(tau.pt());
      if(l1isotauEtaEffDenom_) l1isotauEtaEffDenom_->Fill(tau.eta());
      if(l1isotauPhiEffDenom_) l1isotauPhiEffDenom_->Fill(tau.phi());
    }
    if (!refC.met.empty())
      if(l1etmEtEffDenom_) l1etmEtEffDenom_->Fill(refC.met[0].pt());
  }

  //Analyze L1 Objects (Tau+Jets)
  edm::Handle<l1t::TauBxCollection> taus;
  iEvent.getByToken(l1stage2TausToken_, taus);

  edm::Handle<l1t::EtSumBxCollection> sums;
  iEvent.getByToken(l1stage2SumsToken_, sums);

  LVColl pathTaus;
  LVColl pathIsoTaus;

  //Set Variables for the threshold plot
  LVColl l1taus;
  LVColl l1isotaus;
  LVColl l1met;

  if (taus.isValid()) {
    for (auto const& i : *taus) {
      l1taus.push_back(i.p4());
      if (i.hwIso() > 0)
        l1isotaus.push_back(i.p4());
      if (!doRefAnalysis_) {
        if(l1tauEt_) l1tauEt_->Fill(i.et());
        if(l1tauEta_) l1tauEta_->Fill(i.eta());
        if(l1tauPhi_) l1tauPhi_->Fill(i.phi());
        pathTaus.push_back(i.p4());

        if(l1isotauEt_) l1isotauEt_->Fill(i.et());
        if(l1isotauEta_) l1isotauEta_->Fill(i.eta());
        if(l1isotauPhi_) l1isotauPhi_->Fill(i.phi());
        if (i.hwIso() > 0)
          pathIsoTaus.push_back(i.p4());
      }
    }
  } else {
    edm::LogWarning("HLTTauDQMOffline") << "HLTTauDQML1Plotter::analyze: unable to read L1 tau collection "
                                        << l1stage2Taus_.encode();
  }

  if (sums.isValid() && sums.product()->size() > 0) {
    if (!doRefAnalysis_) {
      for (int ibx = sums->getFirstBX(); ibx <= sums->getLastBX(); ++ibx) {
        for (auto it = sums->begin(ibx); it != sums->end(ibx); it++) {
          auto type = static_cast<int>(it->getType());
          if (type == l1t::EtSum::EtSumType::kMissingEt){
            if(l1etmEt_) l1etmEt_->Fill(it->et());
	  }
        }
      }
    }
  } else {
    edm::LogWarning("HLTTauDQMOffline") << "HLTTauDQML1Plotter::analyze: unable to read L1 met collection "
                                        << l1stage2Sums_.encode();
  }

  //Now do the efficiency matching
  if (doRefAnalysis_) {
    for (auto const& tau : refC.taus) {
      std::pair<bool, LV> m = match(tau, l1taus, matchDeltaR_);
      if (m.first) {
        if(l1tauEt_) l1tauEt_->Fill(m.second.pt());
        if(l1tauEta_) l1tauEta_->Fill(m.second.eta());
        if(l1tauPhi_) l1tauPhi_->Fill(m.second.phi());

        if(l1tauEtEffNum_) l1tauEtEffNum_->Fill(tau.pt());
        if(l1tauHighEtEffNum_) l1tauHighEtEffNum_->Fill(tau.pt());
        if(l1tauEtaEffNum_) l1tauEtaEffNum_->Fill(tau.eta());
        if(l1tauPhiEffNum_) l1tauPhiEffNum_->Fill(tau.phi());

        if(l1tauEtRes_) l1tauEtRes_->Fill((m.second.pt() - tau.pt()) / tau.pt());

        pathTaus.push_back(m.second);
      }
      m = match(tau, l1isotaus, matchDeltaR_);
      if (m.first) {
        if(l1isotauEt_) l1isotauEt_->Fill(m.second.pt());
        if(l1isotauEta_) l1isotauEta_->Fill(m.second.eta());
        if(l1isotauPhi_) l1isotauPhi_->Fill(m.second.phi());

        if(l1isotauEtEffNum_) l1isotauEtEffNum_->Fill(tau.pt());
        if(l1isotauEtaEffNum_) l1isotauEtaEffNum_->Fill(tau.eta());
        if(l1isotauPhiEffNum_) l1isotauPhiEffNum_->Fill(tau.phi());

        if(l1isotauEtRes_) l1isotauEtRes_->Fill((m.second.pt() - tau.pt()) / tau.pt());

        pathIsoTaus.push_back(m.second);
      }
    }

    if (sums.isValid() && sums.product()->size() > 0) {
      for (int ibx = sums->getFirstBX(); ibx <= sums->getLastBX(); ++ibx) {
        for (auto it = sums->begin(ibx); it != sums->end(ibx); it++) {
          auto type = static_cast<int>(it->getType());
          if (type == l1t::EtSum::EtSumType::kMissingEt) {
            if(l1etmEt_) l1etmEt_->Fill(it->et());
            if(l1etmPhi_) l1etmPhi_->Fill(it->phi());

            if (it->et() > l1ETMMin_) {
              if(l1etmEtEffNum_) l1etmEtEffNum_->Fill(it->et());
            }
          }
        }
      }
    }
  }

  //Fill the Threshold Monitoring
  if (pathTaus.size() > 1)
    std::sort(pathTaus.begin(), pathTaus.end(), [](const LV& a, const LV& b) { return a.pt() > b.pt(); });
  if (!pathTaus.empty()) {
    if(firstTauEt_) firstTauEt_->Fill(pathTaus[0].pt());
    if(firstTauEta_) firstTauEta_->Fill(pathTaus[0].eta());
    if(firstTauPhi_) firstTauPhi_->Fill(pathTaus[0].phi());
  }
  if (pathTaus.size() > 1) {
    if(secondTauEt_) secondTauEt_->Fill(pathTaus[1].pt());
    if(secondTauEta_) secondTauEta_->Fill(pathTaus[1].eta());
    if(secondTauPhi_) secondTauPhi_->Fill(pathTaus[1].phi());
  }
  if (pathIsoTaus.size() > 1)
    std::sort(pathIsoTaus.begin(), pathIsoTaus.end(), [](const LV& a, const LV& b) { return a.pt() > b.pt(); });
  if (!pathIsoTaus.empty()) {
    if(firstIsoTauEt_) firstIsoTauEt_->Fill(pathIsoTaus[0].pt());
    if(firstIsoTauEta_) firstIsoTauEta_->Fill(pathIsoTaus[0].eta());
    if(firstIsoTauPhi_) firstIsoTauPhi_->Fill(pathIsoTaus[0].phi());
  }
  if (pathIsoTaus.size() > 1) {
    if(secondIsoTauEt_) secondIsoTauEt_->Fill(pathIsoTaus[1].pt());
    if(secondIsoTauEta_) secondIsoTauEta_->Fill(pathIsoTaus[1].eta());
    if(secondIsoTauPhi_) secondIsoTauPhi_->Fill(pathIsoTaus[1].phi());
  }
}
