read_qdelay_file = open("/home/meenakshy/tata/TCPEvalOutput/Accesslink/EXPT-1/ns3::TcpNewReno_qdel.dat", "r")
read_throughput_file = open("/home/meenakshy/tata/TCPEvalOutput/Accesslink/EXPT-1/ns3::TcpNewReno_throughput.dat", "r") 
qdelay_lines = read_qdelay_file.readlines()
throughput_lines = read_throughput_file.readlines()
last_time_interval = 0.0
qdelay_ans = 0.0
qdelay_count = 0
throughput_index = 0
throughput_val = 0
throughput_count = 0
for l in qdelay_lines:
  s = l.split(' ')
  qdelay_time = float(s[0])
  qdelay_val = float(s[1])
  curr_time_interval = ((int)(qdelay_time*10))/10.0
  if last_time_interval == curr_time_interval:
    qdelay_ans += qdelay_val
    qdelay_count += 1
  else:
    if not qdelay_count == 0:
        qdelay_ans /= qdelay_count
        while throughput_index<len(throughput_lines):
            throughput_time = float((throughput_lines[throughput_index].split(' '))[0])
            if throughput_time < curr_time_interval:
                throughput_val += float((throughput_lines[throughput_index].split(' '))[1])
                throughput_count +=1
                throughput_index +=1
            else:
                break
        throughput_ans = throughput_val/throughput_count
        print qdelay_ans, throughput_ans
        throughput_val = 0
        throughput_count = 0
        qdelay_ans = 0
        qdelay_count = 0
    last_time_interval = curr_time_interval

if not qdelay_count == 0: 
    qdelay_ans /=  qdelay_count
    while throughput_index<len(throughput_lines):
        throughput_val += float((throughput_lines[throughput_index].split(' '))[1])
        throughput_count +=1
        throughput_index +=1
    throughput_ans = throughput_val/throughput_count
    print qdelay_ans, throughput_ans
    throughput_val = 0
    throughput_count = 0
    qdelay_ans = 0
    qdelay_count = 0
last_time_interval = curr_time_interval
