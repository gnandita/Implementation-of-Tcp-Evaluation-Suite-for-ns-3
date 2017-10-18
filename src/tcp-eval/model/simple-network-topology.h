/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016 NITK Surathkal
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

// Define an object to create a simple-network topology in tcp-eval.

#ifndef SIMPLE_NETWORK_TOPOLOGY_H
#define SIMPLE_NETWORK_TOPOLOGY_H

#include <stdint.h>

#include "configure-topology.h"
#include "traffic-parameters.h"
#include "create-traffic.h"
#include "evaluator.h"
namespace ns3 {

class SimpleNetworkTopology : public ConfigureTopology
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * \brief Constructor
   */
  SimpleNetworkTopology (void);

  /**
   * \brief Destructor
   */
  ~SimpleNetworkTopology (void);

  void DestroyTrace (Evaluator et);

  /**
   * \brief Converts a value to string.
   *
   * This method is used because std::to_string() works with c++11
   * and std::itoa is not a standard library
   *
   * \param data The value which is to be converted to string.
   */
  template <typename T>
  std::string to_string (const T& data);

  /**
   * \brief Set delay of cross links in seconds
   *
   * \param crossLinkDelay delay of cross links in seconds
   */
  void SetCrossLinkDelay (Time crossLinkDelay);

  /**
   * \brief Get delay of cross links in seconds
   *
   * \return delay of cross links in seconds
   */
  Time GetCrossLinkDelay (void) const;

  void CreateSimpleNetworkTopology (Ptr<TrafficParameters> traffic, std::string fileName);

private:
  Time     m_crossLinkDelay;      //!< Cross link Delay in seconds
};

}

#endif /*SIMPLE_NETWORK_TOPOLOGY_H*/
