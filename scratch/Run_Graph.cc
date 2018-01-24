#include <stdlib.h>
#include <string>
#include <iostream>
#include <dirent.h>
#include "ns3/core-module.h"

using namespace ns3;

int main (int argc, char *argv[])
{
  std::string scenarioName = "simple-network";
  std::string useAqm = "false"; //use bool
  std::string queueName;
  std::string Aqm_Name = "";
  CommandLine cmd;
  cmd.AddValue ("scenarioName", "Scenario Name ", scenarioName);
  cmd.AddValue ("useAqm", "use Aqm", useAqm);
  cmd.AddValue ("Aqm_Name", "Name of the AQM if use Aqm is true", Aqm_Name);
  cmd.Parse (argc,argv);
  if (useAqm == "true")
    {
      if (Aqm_Name == "RedQueueDisc" ||  Aqm_Name == "PieQueueDisc" || Aqm_Name == "CoDelQueueDisc"|| Aqm_Name == "FqCoDelQueueDisc")
        {
          queueName = Aqm_Name;
        }
      else
        {
          std::cout << "Invalid AQM Name";
          exit (1);
        }
    }
  else
    {
      queueName = "PfifoFastQueueDisc";
    }
  std::string set_dest = "python set_result_dir.py";
  system (set_dest.c_str ());
  std::ifstream infile;
  infile.open ("latest_dir.dat");
  std::string default_directory;
  infile >> default_directory;
  uint32_t i = 0,expt_num, num_Tcp;
  std::string transport_prot[] = {"TcpHybla","TcpHighSpeed","TcpHtcp","TcpVegas","TcpScalable","TcpVeno","TcpBic","TcpYeah", "TcpIllinois","TcpWestwood","TcpWestwoodPlus", "TcpLedbat","TcpNewReno"};
  std::string run = "";
  num_Tcp = 2;

  for (expt_num = 1; expt_num <= 1; expt_num++)
    {
      for (i = 0; i < num_Tcp; i++)
        {
          if (useAqm == "false")
            {
              run = "./waf --run \"drive-" + scenarioName + " " + "--useAqm=" + useAqm + " " + "--tcp_variant=" + transport_prot[i] + "\"";
            }
          else
            {
              run = "./waf --run \"drive-" + scenarioName + " " + "--useAqm=" + useAqm + " " + "--tcp_variant=" + transport_prot[i] + " " + "--Aqm_Name=" + Aqm_Name + "\"";
            }
          system (run.c_str ());
          std::string proQdelThr = std::string ("python src/tcp-eval/generate-ellipseinput.py ") + scenarioName + " " + transport_prot[i] + " " +       queueName + " " + default_directory;
          std::string proEllipse = std::string ("python src/tcp-eval/ellipsemaker ") + scenarioName + " " + transport_prot[i] + " " + queueName        + " " + default_directory;
          std::string Result = proQdelThr;
          std::string Ellipse = proEllipse;
          system (Result.c_str ());
          system (Ellipse.c_str ());
        }
    }
  std::string ProPerFlow = std::string ("python src/tcp-eval/generate-perflow.py ") + scenarioName + " " + default_directory;
  std::string PerFlow = ProPerFlow;
  system (PerFlow.c_str ());
  return(0);
}
