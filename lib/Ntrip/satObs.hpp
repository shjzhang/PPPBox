#ifndef SATOBS_HPP
#define SATOBS_HPP

#include <string>
#include <vector>

#include "CommonTime.hpp"
#include "RinexSatID.hpp"

// the observation on one frequency

class t_frqObs  {
 public:
  t_frqObs() {
    _code            = 0.0;
    _codeValid       = false;
    _phase           = 0.0;
    _phaseValid      = false;
    _doppler         = 0.0;
    _dopplerValid    = false;
    _snr             = 0.0;
    _snrValid        = false;
    _slip            = false;
    _slipCounter     = 0;
    //_biasJumpCounter = 0;
  }
  std::string       _rnxType2ch;
  double            _code;
  bool              _codeValid;
  double            _phase;
  bool              _phaseValid;
  double            _doppler;
  bool              _dopplerValid;
  double            _snr;
  bool              _snrValid;
  bool              _slip;
  int               _slipCounter;
  //int               _biasJumpCounter;
};

// the observations on one satellite
class t_satObs {
 public:
  t_satObs() {}
  t_satObs(const t_satObs& old) { // copy constructor (deep copy)
    _staID = old._staID;
    _prn   = old._prn;
    _time  = old._time;
    for (unsigned ii = 0; ii < old._obs.size(); ii++) {
      _obs.push_back(new t_frqObs(*old._obs[ii]));
    }
  }
  /**
   * Destructor of satellite measurement storage class
   */
  ~t_satObs()
  {
    clear();
  }

  /**
   * Cleanup function resets all elements to initial state.
   */
  inline void clear(void)
  {
    for (unsigned ii = 0; ii < _obs.size(); ii++)
      delete _obs[ii];
    _obs.clear();
    _obs.resize(0);
    //_time.reset();
    //_prn.clear();
    _staID.clear();
  }

  std::string            _staID;
  gpstk::RinexSatID      _prn;
  gpstk::CommonTime      _time;
  std::vector<t_frqObs*> _obs;
};

#endif // SATOBS_HPP
