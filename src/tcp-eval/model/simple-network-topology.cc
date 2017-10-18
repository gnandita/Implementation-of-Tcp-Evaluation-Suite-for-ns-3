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

// Implement an object to create simple-network topology in tcp-eval.

#include <iostream>
#include <cstdlib>
#include <sstream>
#include <string>

#include "simple-network-topology.h"
//#include "eval-stats.h"
#include "evaluator.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/traffic-control-module.h"
#include "point-to-point-simple-network.h"
namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("SimpleNetworkTopology");

NS_OBJECT_ENSURE_REGISTERED (SimpleNetworkTopology);

TypeId
SimpleNetworkTopology::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SimpleNetworkTopology")
    .SetParent<ConfigureTopology> ()
    .SetGroupName ("TcpEvaluationSuite")
    .AddAttribute ("CrossLinkDelay",
                   "Delay of cross links in seconds",
                   TimeValue (Seconds (10)),
                   MakeTimeAccessor (&SimpleNetworkTopology::m_crossLinkDelay),
                   MakeTimeChecker ())
  ;
  return tid;
}

SimpleNetworkTopology::SimpleNetworkTopology (void)
{
}

SimpleNetworkTopology::~SimpleNetworkTopology (void)
{
}

void
SimpleNetworkTopology::SetCrossLinkDelay (Time crossLinkDelay)
{
  m_crossLinkDelay = crossLinkDelay;
}

Time
SimpleNetworkTopology::GetCrossLinkDelay (void) const
{
  return m_crossLinkDelay;
}

void
SimpleNetworkTopology::DestroyTrace (Evaluator et)
{
  et.DestroyConnection ();
}

void
SimpleNetworkTopology::CreateSimpleNetworkTopology (Ptr<TrafficParameters> trafficParams, std::string fileName)
{
  // Set default parameters for topology
  SetTopologyParameters (trafficParams, BottleneckCount ());

  std::string m_aqm;
  TrafficControlHelper tchBottleneck;
  Ptr<QueueDisc> m_queue;

  PointToPointHelper pointToPointAccessToCoreRouter, pointToPointLeaf, pointToPointCrossLinks,pointToPointCoreRouter;
  pointToPointAccessToCoreRouter.SetDeviceAttribute  ("DataRate", StringValue (to_string<double> (m_bottleneckBandwidth) + std::string ("Mbps")));
  pointToPointAccessToCoreRouter.SetChannelAttribute ("Delay", StringValue (to_string<double> (m_bottleneckDelay.ToDouble (Time::S)) + std::string ("s")));

  pointToPointLeaf.SetDeviceAttribute  ("DataRate", StringValue (to_string<double> (m_nonBottleneckBandwidth) + std::string ("Mbps")));
  pointToPointLeaf.SetChannelAttribute ("Delay", StringValue (to_string<double> (m_nonBottleneckDelay.ToDouble (Time::S)) + std::string ("s")));

  pointToPointCrossLinks.SetDeviceAttribute  ("DataRate", StringValue (to_string<double> (m_nonBottleneckBandwidth) + std::string ("Mbps")));
  pointToPointCrossLinks.SetChannelAttribute ("Delay", StringValue (to_string<double> (m_crossLinkDelay.ToDouble (Time::S)) + std::string ("s")));

  pointToPointCoreRouter.SetDeviceAttribute  ("DataRate", StringValue (to_string<double> (m_coreLinkBandwidth) + std::string ("Mbps")));
  pointToPointCoreRouter.SetChannelAttribute ("Delay", StringValue (to_string<double> (m_coreLinkDelay.ToDouble (Time::S)) + std::string ("s")));

  uint32_t nFlow;
  uint32_t nFwdFtpFlow = trafficParams->GetNumOfFwdFtpFlows ();
  uint32_t nRevFtpFlow = trafficParams->GetNumOfRevFtpFlows ();
  uint32_t nVoiceFlow = trafficParams->GetNumOfVoiceFlows ();
  uint32_t nFwdStreamingFlow = trafficParams->GetNumOfFwdStreamingFlows ();
  uint32_t nRevStreamingFlow = trafficParams->GetNumOfRevStreamingFlows ();

  // Calculate total leaf nodes at each side
  uint32_t nLeftLeaf = nFwdFtpFlow + nRevFtpFlow + nVoiceFlow + nFwdStreamingFlow + nRevStreamingFlow;
  uint32_t nRightLeaf = nLeftLeaf;

  uint32_t nCrossFtpFlow = trafficParams->GetNumOfCrossFtpFlows ();

  PointToPointSimpleNetworkHelper simpleNetwork (nLeftLeaf, pointToPointLeaf,
                                                 nRightLeaf, pointToPointLeaf,
                                                 pointToPointAccessToCoreRouter,
                                                 pointToPointAccessToCoreRouter,
                                                 pointToPointCoreRouter, nCrossFtpFlow,
                                                 pointToPointCrossLinks);
  InternetStackHelper stack;
  simpleNetwork.InstallStack (stack);
  if (trafficParams->IsAqmUsed () == true)
    {

      SetAqmParameters (trafficParams->GetAqmName ());
      m_aqm = "ns3::" + trafficParams->GetAqmName ();
      tchBottleneck.SetRootQueueDisc (m_aqm);
      std::cout << "err";
      for (uint32_t k = 0; k < 4; k++)
        {
          tchBottleneck.Install (simpleNetwork.GetCoreRouter (k)->GetDevice (0));

        }
      tchBottleneck.Install (simpleNetwork.GetLeftAccessRouter ()->GetDevice (0));
      tchBottleneck.Install (simpleNetwork.GetRightAccessRouter ()->GetDevice (0));
    }
  else
    {
      m_aqm = "ns3::PfifoFastQueueDisc";
      tchBottleneck.SetRootQueueDisc ("ns3::PfifoFastQueueDisc", "Limit", UintegerValue (m_nonBottleneckBuffer));
      for (uint32_t k = 0; k < 4; k++)
        {
          tchBottleneck.Install (simpleNetwork.GetCoreRouter (k)->GetDevice (0));
        }
      tchBottleneck.Install (simpleNetwork.GetLeftAccessRouter ()->GetDevice (0));
      tchBottleneck.Install (simpleNetwork.GetRightAccessRouter ()->GetDevice (0));
    }
  tchBottleneck.Uninstall (simpleNetwork.GetLeftAccessRouter ()->GetDevice (0));
  m_queue = tchBottleneck.Install (simpleNetwork.GetLeftAccessRouter ()->GetDevice (0)).Get (0);
  simpleNetwork.AssignIpv4Addresses (Ipv4AddressHelper ("10.1.1.0", "255.255.255.0"),
                                     Ipv4AddressHelper ("10.10.1.0", "255.255.255.0"),
                                     Ipv4AddressHelper ("10.50.1.0", "255.255.255.0"),
                                     Ipv4AddressHelper ("10.100.1.0", "255.255.255.0"),
                                     Ipv4AddressHelper ("10.150.1.0", "255.255.255.0"),
                                     Ipv4AddressHelper ("10.170.1.0", "255.255.255.0"));

  // offset helps in iterating over the topology by keeping track of
  // the nodes created for a particular traffic
  uint32_t offset = 0;
  Ptr<CreateTraffic> createTraffic = CreateObject<CreateTraffic> ();
  if (nFwdFtpFlow > 0)
    {
      // Create forward FTP traffic
      createTraffic->CreateFwdFtpTrafficSimpleNetwork (simpleNetwork, nFwdFtpFlow, offset, trafficParams);
      offset += nFwdFtpFlow;
      nFlow = nFwdFtpFlow;
    }

  if (nRevFtpFlow > 0)
    {
      // Create reverse FTP traffic
      createTraffic->CreateRevFtpTrafficSimpleNetwork (simpleNetwork, nRevFtpFlow, offset, trafficParams);
      offset += nRevFtpFlow;
      nFlow = nRevFtpFlow;
    }

  if (trafficParams->GetNumOfCrossFtpFlows () > 0)
    {
      // Create cross FTP traffic
      createTraffic->CreateCrossFtpTrafficSimpleNetwork (simpleNetwork, nCrossFtpFlow, trafficParams);
    }

  if (nVoiceFlow > 0)
    {
      // Create voice traffic
      createTraffic->CreateVoiceTrafficSimpleNetwork (simpleNetwork, nVoiceFlow, offset, trafficParams);
      offset += nVoiceFlow;
      nFlow = nVoiceFlow;
    }
  if (nFwdStreamingFlow > 0)
    {
      // Create forward streaming traffic
      createTraffic->CreateFwdStreamingTrafficSimpleNetwork (simpleNetwork, nFwdStreamingFlow, offset, trafficParams);
      offset += nFwdStreamingFlow;
      nFlow = nFwdStreamingFlow;
    }
  if (nRevStreamingFlow > 0)
    {
      // Create reverse streaming traffic
      createTraffic->CreateRevStreamingTrafficSimpleNetwork (simpleNetwork, nRevStreamingFlow, offset, trafficParams);
      offset += nRevStreamingFlow;
      nFlow = nRevStreamingFlow;
    }

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  // Push the stats of left most router to a file
  Ptr<Node> left = simpleNetwork.GetLeftAccessRouter ();
  Evaluator et = Evaluator ("simple-network",nFlow,m_aqm,trafficParams->GetTcpVarient (),trafficParams->GetStreamingPacketSize (),m_queue,left);
  Simulator::Schedule (trafficParams->GetSimulationTime () /*simtime*/, &SimpleNetworkTopology::DestroyTrace, this, et);
  Simulator::Stop (Time::FromDouble (((trafficParams->GetSimulationTime ()).ToDouble (Time::S) + 5), Time::S));
  Simulator::Run ();
  Simulator::Destroy ();
}

template <typename T>
std::string SimpleNetworkTopology::to_string (const T& data)
{
  std::ostringstream conv;
  conv << data;
  return conv.str ();
}

}
