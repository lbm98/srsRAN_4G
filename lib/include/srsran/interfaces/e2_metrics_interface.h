/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/hdr/stack/ric/e2sm.h"
#include "srsran/common/common.h"
#include "srsran/common/interfaces_common.h"
#include "srsran/interfaces/enb_metrics_interface.h"
#include "srsran/srsran.h"

#ifndef SRSRAN_E2_INTERFACES_H
#define SRSRAN_E2_INTERFACES_H

namespace srsenb {

class e2_interface_metrics
{
public:
  virtual bool pull_metrics(enb_metrics_t* m) = 0;

  virtual bool register_e2sm(e2sm* sm)   = 0;
  virtual bool unregister_e2sm(e2sm* sm) = 0;
};

} // namespace srsenb

#endif // SRSRAN_ENB_INTERFACES_H
