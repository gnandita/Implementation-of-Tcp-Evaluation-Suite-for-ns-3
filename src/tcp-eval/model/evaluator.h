/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017 NITK Surathkal
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
 * Authors: Ankit Deepak <adadeepak8@gmail.com>
 *          Shravya K. S. <shravya.ks0@gmail.com>
 *          Mohit P. Tahiliani <tahiliani@nitk.edu.in>
 */

#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <fstream>
//#include <ofstream>
#include <vector>
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/traffic-control-module.h"
#include "ns3/assert.h"
#include "ns3/data-rate.h"
#include "eval-app.h"

namespace ns3 {

class Evaluator
{

public:

  static TypeId GetTypeId (void);

  Evaluator (std::string ScenarioName,
                      uint32_t numFlows,
                      std::string queueDisc,std::string tcp_varient,
                      uint32_t pktSize,Ptr<QueueDisc> queue,Ptr<Node> node);

  /**
   * \brief Destructor
   */
  ~Evaluator ();

  
  
  void DestroyConnection ();


/*  void Pause (uint32_t flow)
    {
      m_sources[flow]->CancelEvents ();
    }
*/
  /**
   * \brief Restarts the paused flow
   *
   * \param flow The flow id
   */
 /* void Restart (uint32_t flow)
    {
      m_sources[flow]->ScheduleStartEvent ();
    }
*/
private:
  
  void PacketEnqueue (Ptr<const QueueDiscItem> item);

  
  void PacketDequeue (Ptr<const QueueDiscItem> item);

  
  void PacketDrop (Ptr<const QueueDiscItem> item);

  
  void PayloadSize (Ptr<const Packet> packet/*, const Address & address*/);

  std::string m_currentAQM;                       //!< AQM being currently simulated
  uint32_t m_numFlows;                            //!< The total number of flows
  uint32_t m_flowsAdded;                          //!< The number of flows added till now in the simulation
  //PointToPointDumbbellHelper m_dumbbell;          //!< Point to point dumbbell helper object
  Ptr<QueueDisc> m_queue;                         //!< The queue discipline installed on the bottleneck link
  uint32_t m_packetSize;                          //!< The size of payload
  uint64_t m_QDrecord;                            //!< The sum of queue delays observed in 10ms
  uint64_t m_numQDrecord;                         //!< The number of samples obtained in 10ms
  Time m_lastQDrecord;                            //!< Last time the average queue delay was calculated
  Ptr<OutputStreamWrapper> m_QDfile;              //!< File to store queue delay values
  uint64_t m_TPrecord;                            //!< The total amount of data received in 10ms
  Time m_lastTPrecord;                            //!< Last time the average goodput was calculated
  Ptr<OutputStreamWrapper> m_TPfile;              //!< File to store goodput values
  std::vector < Ptr < PacketSink >> m_sinks;      //!< List of packet sinks
  Ptr<OutputStreamWrapper> m_GPfile;              //!< File to store per flow statistics
  std::vector < Ptr < EvalApp >> m_sources;       //!< List of application sources
  Ptr<OutputStreamWrapper> m_metaData;            //!< File to store flow completion times
  Ptr<OutputStreamWrapper> m_dropTime;            //!< File to store packet drop times
  Ptr<OutputStreamWrapper> m_enqueueTime;         //!< File to store packet enqueue times
  Ptr<PointToPointNetDevice> m_device;
};

}

#endif /* EVALUATOR_H */
