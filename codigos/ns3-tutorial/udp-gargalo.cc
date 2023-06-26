/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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
 * Changed by : Claudia Jacy Barenco Abbas 
 */

#include <fstream>
#include <vector>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-helper.h"              //Para uso do FlowMonitor
#include "ns3/flow-monitor.h"
#include "ns3/ipv4-flow-classifier.h"             //Para classificador do fluxo
#include "ns3/gnuplot.h"                          //Para GnuPlot
#include "ns3/ipv4-l3-protocol.h"
#include "ns3/traffic-control-module.h"
#include "ns3/mobility-helper.h"
#include "ns3/mobility-module.h"
#include "ns3/netanim-module.h"

// Network Topology
// 
//
//             p2p              p2p               p2p
//       n0 --------- n1 ------------------ n2 --------- n3
// (client)                                              (server)
//        

using namespace ns3;


NS_LOG_COMPONENT_DEFINE ("UDPGargalo");

Gnuplot2dDataset dataset1;

std::ofstream saida;

void P2PTXQueueNPackets(uint32_t oldValue, uint32_t newValue){
  double time = Simulator::Now().GetSeconds();

  dataset1.Add(time, newValue);
  std::cout<< "Pacotes na fila: " << oldValue<< "->" << newValue << "\n";

  saida << time << ";" << oldValue<< ";" << newValue << "\n";

}

void 
Create2DPlotFile (Gnuplot2dDataset GnuDataset, std::string fileName, std::string plotTitle, std::string dataTitle, std::string xLegend,
std::string yLegend, std::string xRange, std::string yRange) {
  std::string fileNameWithNoExtension = fileName;
  std::string graphicsFileName  = fileNameWithNoExtension + ".png";
  std::string plotFileName = fileNameWithNoExtension + ".plt";

  // Instantiate the plot and set its title.
  Gnuplot plot (graphicsFileName);
  plot.SetTitle (plotTitle);

  // Make the graphics file, which the plot file will create when it
  // is used with Gnuplot, be a PNG file.
  plot.SetTerminal ("png");

  // Set the labels for each axis.
  plot.SetLegend (xLegend, yLegend);

  // Set the range for the x axis.
  plot.AppendExtra ("set xrange "+xRange);

  // Set the range for the y axis.
  plot.AppendExtra ("set yrange "+yRange);

  // Instantiate the dataset1, set its title, and make the points be
  // plotted along with connecting lines.
  GnuDataset.SetTitle (dataTitle);
  GnuDataset.SetStyle (Gnuplot2dDataset::STEPS);

  // Add the dataset1 to the plot.
  plot.AddDataset (GnuDataset);

  // Open the plot file.
  std::ofstream plotFile (plotFileName.c_str());

  // Write the plot file.
  plot.GenerateOutput (plotFile);

  // Close the plot file.
  plotFile.close ();
}

int
main (int argc, char *argv[])
{

  saida.open("/home/ubuntu/ns-allinone-3.38/ns-3.38/scratch/adr/exp02-udpgargalo.csv");
  
  // Parse de Comandos
  std::string DataRate="10kbps";
  std::string Delay="5ms";

  uint32_t time = 100;
  uint32_t nPackets=1000000;
  uint32_t MaxPacketSize = 1024;
  CommandLine cmd;  

  // Alteração do Número de pacotes a serem transmitidos
  cmd.AddValue("nPackets","Número de pacotes para enviar no echo.",nPackets);

  // Alteração da Taxa de Dados na Camada de Enlace  
  cmd.AddValue("DataRate","Taxa de dados no enlace Ponto a Ponto Backbone.",DataRate);
  cmd.AddValue("Delay","Atraso de Propagação no enlace Ponto a Ponto Backbone.",Delay);
  cmd.AddValue("Time","Tempo de Simulação.",time);
  cmd.AddValue("MaxPacketSize","Tamanho máximo dos pacotes.",MaxPacketSize);

  cmd.Parse (argc, argv);

  Time::SetResolution (Time::NS); 

  // Cria os nós
  NodeContainer nodes;
  nodes.Create(4);

  // Define os canais
  PointToPointHelper accessLink1, bottleneckLink, accessLink2;

  accessLink1.SetDeviceAttribute ("DataRate", StringValue ("100kbps"));
  accessLink1.SetChannelAttribute ("Delay", StringValue ("2ms"));

  bottleneckLink.SetDeviceAttribute ("DataRate", StringValue(DataRate));
  bottleneckLink.SetChannelAttribute ("Delay", StringValue(Delay));

  accessLink2.SetDeviceAttribute ("DataRate", StringValue ("100kbps"));
  accessLink2.SetChannelAttribute ("Delay", StringValue ("2ms"));

  // Define os devices -> instalação dos enlaces nos nós
  NetDeviceContainer devicesAccessLink1, devicesBottleneckLink, devicesAccessLink2;
  devicesAccessLink1 = accessLink1.Install(nodes.Get(0), nodes.Get(1));
  devicesBottleneckLink = bottleneckLink.Install(nodes.Get(1), nodes.Get(2));
  devicesAccessLink2 = accessLink2.Install(nodes.Get(2), nodes.Get(3));

  InternetStackHelper stack;
  stack.InstallAll();

  // Cria interfaces -> Atribuição dos enderços IP em devices
  Ipv4AddressHelper address;
  address.SetBase ("10.0.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfacesAccess1 = address.Assign (devicesAccessLink1);
  address.SetBase ("10.0.2.0", "255.255.255.0");
  Ipv4InterfaceContainer interfacesBottleneck = address.Assign (devicesBottleneckLink);
  address.SetBase ("10.0.3.0", "255.255.255.0");
  Ipv4InterfaceContainer interfacesAccess2 = address.Assign (devicesAccessLink2);
  
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  // Cria aplicação servidora no nó 3, porta 4000
  uint16_t port = 4000;
  UdpServerHelper server (port);
  ApplicationContainer apps = server.Install (nodes.Get(3));
  apps.Start (Seconds (0.0));
  apps.Stop (Seconds (time));
  
  // Cria duas aplicações clientes
  Time interPacketInterval = Seconds (0.187);
  uint32_t maxPacketCount = nPackets;

  UdpEchoClientHelper client1 (interfacesAccess2.GetAddress (1), port);
  client1.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
  client1.SetAttribute ("Interval", TimeValue (interPacketInterval));
  client1.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));
  apps = client1.Install (nodes.Get(0));
  apps.Start (Seconds (0.0));
  apps.Stop (Seconds (time-10));

  MobilityHelper mobility;
  mobility.SetPositionAllocator("ns3::GridPositionAllocator",
    "MinX", DoubleValue(0.0),
    "MinY", DoubleValue(0.0),
    "DeltaX", DoubleValue(5.0),
    "DeltaY", DoubleValue(10.0),
    "GridWidth", UintegerValue(4),
    "LayoutType", StringValue("RowFirst"));

    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);

  // P2P TX Queue Tracing
  Config::ConnectWithoutContext("/NodeList/1/DeviceList/*/$ns3::PointToPointNetDevice/TxQueue/PacketsInQueue", MakeCallback(&P2PTXQueueNPackets));

  // Habilita rastreamento de tráfego em formato ASCII
  AsciiTraceHelper ascii;
  bottleneckLink.EnableAsciiAll(ascii.CreateFileStream("UDPGargalo.tr"));

  // Habilita rastreamento de tráfego em formato PCAP
  bottleneckLink.EnablePcap ("N1UDPGargalo", devicesBottleneckLink.Get(0));
  bottleneckLink.EnablePcap ("N3UDPGargalo", devicesAccessLink1.Get(0));

  // Instala FlowMonitor em todos os nós 
  Ptr<FlowMonitor> flowMonitor;
  FlowMonitorHelper flowHelper;
  flowMonitor = flowHelper.InstallAll ();

  // Define tempo de simulação 
  Simulator::Stop (Seconds (time));

  // Executa simulação
  Simulator::Run ();

  flowMonitor->CheckForLostPackets ();

  // Escreve estatísticas dos fluxos em um arquivo XML 
  flowMonitor->SerializeToXmlFile("UDPGargalo.xml", true, true);

  // Mostra estatísticas por fluxo
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowHelper.GetClassifier ());
  std::map<FlowId, FlowMonitor::FlowStats> stats = flowMonitor->GetFlowStats ();
  
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
      Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
      std::cout << "Flow " << i->first << " (" << t.sourceAddress << ":" << t.sourcePort << " -> "<< t.destinationAddress << ":" << t.destinationPort << ")\n";
      std::cout << "  Tx Pacotes: " << i->second.txPackets << "\n";
      std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
      std::cout << "  Tx Taxa de bits média:  " << ((i->second.txBytes * 8.0)/((i->second.timeLastTxPacket.GetSeconds()-i->second.timeFirstTxPacket.GetSeconds())))  << " bps\n";
      std::cout << "  Rx Pacotes: " << i->second.rxPackets << "\n";
      std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
      std::cout << "  Rx Taxa de bits média: " << ((i->second.rxBytes * 8.0)/ ((i->second.timeLastRxPacket.GetSeconds()-i->second.timeFirstRxPacket.GetSeconds())))  << " bps\n";
      std::cout << "  Throughput médio (simulação): " << (i->second.rxBytes * 8.0)/time << " bps\n";
      std::cout << "  Atraso médio: " << i->second.delaySum.GetSeconds()/i->second.rxPackets << " s\n";
      std::cout << "  Jitter médio: " << 1000 * (i->second.jitterSum.GetSeconds()/(i->second.rxPackets -1)) << " ms\n";
      std::cout << "  Número de pacotes perdidos: " << i->second.lostPackets << "\n";

    }

  Create2DPlotFile (dataset1, "UDPGargaloFila", "Tamanho da Fila x Tempo de Simulação", "Pacotes na fila", "Tempo de Simulação (s)",
  "Número de pacotes na fila", "[0:100]", "[0:100]");
  
  Simulator::Destroy ();

  saida.close();

  return 0;
}
