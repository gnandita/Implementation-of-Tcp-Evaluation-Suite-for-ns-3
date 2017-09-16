# Implementation of TCP Evaluation Suite for ns-3

## Course Code: CS898

### Overview:

TCP Evaluation Suite helps to compare TCP extensions with each other using metrics such as queue delay and average throughput. Internet draft [1] has proposed a common TCP evaluation suite that specifies the standard scenarios,test cases and metrics for comparision. A TCP evaluation tool for ns-2 is implemented in [2].The paper [3] simulates some of the scenaros described in [1] using ns-3 traffic. We extend this by porting it to ns-3-dev and by adding more scenarios and AQM support. The reference code for the paper [3] is present in [4].

### References:

[1] L. Andrew and S. Floyd. 2014. Common TCP Evaluation Suite. Internet draft-irtf-iccrg-tcpeval-01. Internet Engineering Task Force. https://tools.ietf.org/html/draft-irtf-iccrg-tcpeval-01.

[2] 2007.https://sourceforge.net/projects/tcpeval.

[3] D. Kumar Mishra, P. Vankar, and M. P. Tahiliani. 2016. TCP Evaluation Suite for ns-3. In Proceedings of the Workshop on ns-3. ACM, 25–32.

[4] Code of the paper mentioned in [3] https://github.com/dharmendra-mishra/wns3-2016.
