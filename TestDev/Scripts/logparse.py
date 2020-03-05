#!/usr/bin/python3.5
import sys
import re

c_line = 0
c_pytest_I = 0
c_pytest_D = 0
c_pytest_E = 0
c_remaining = 0
c_logmsg = 0
c_other = 0
tag_set = set([])
tag_pid_set = set([])
prd_debug_print = "^\[[0-9]+\.[0-9]{3} \([0-9]{6}\)"
prd_debug_format = "^(\[(\d+\.\d{3}) (\(\d{6}\)):([^:]+):([^\]]+)\](.*))"
for line in sys.stdin:
    c_line +=1
    line = line.strip()
    if re.search("^\[I ", line):
        c_pytest_I += 1
    elif re.search("^\[D ", line):
        c_pytest_D += 1
    elif re.search("^\[E ", line):
        c_pytest_E += 1
    elif re.search(prd_debug_print, line):
        c_logmsg +=1
        #if (c_logmsg < 10):
        #   print(line)
        match_obj = re.search(prd_debug_format, line)
        #print(match_obj.group(1))   #entire line
        #print(match_obj.group(2))   #time stamp
        #print(match_obj.group(3))   #pid
        #print(match_obj.group(4))   #dprint tag
        tag_set.add(match_obj.group(4))
        pid_tag = match_obj.group(3) + " " + match_obj.group(4)
        tag_pid_set.add(pid_tag)
        #print(match_obj.group(5))   #dprint level
        #print(match_obj.group(6))   #Rest of line

           
    else:
         c_other += 1

c_remaining = c_line

for t in tag_set:
    print(t)
for t in tag_pid_set:
    print(t)
print("c_pytest_I:" + str(c_pytest_I) + " :" + str(round(c_pytest_I/c_line,2)))
print("c_pytest_D:" + str(c_pytest_D) + " :" + str(round(c_pytest_D/c_line,2)))
print("c_pytest_E:" + str(c_pytest_E) + " :" + str(round(c_pytest_E/c_line,2)))
c_remaining -= c_pytest_I + c_pytest_D + c_pytest_E 
print("c_logmsg:" + str(c_logmsg) + " :" + str(round(c_logmsg/c_line,2)))
