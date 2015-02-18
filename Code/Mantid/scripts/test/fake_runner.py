""" Allows autoreduction runs to be kicked off one after another, for testing MD file accumulation """

import sched, time
import subprocess

s = sched.scheduler(time.time, time.sleep)

def print_time(i):
    print "From print_time", time.ctime(), " ", i

def call_reduce(start_time, i, interval_time, kwargs):
    start = time.time()
    subprocess.call(['python', 'MerlinAccumulateMDReduction.py', kwargs['data'], kwargs['output']])
    end = time.time()
    print 'Reduce time = ', end - start, ', leaving ', start_time + (i+1)*interval_time - end, ' second interval.'
    print 'Next run will start at:', time.ctime(start_time + (i+1)*interval_time), '. Last file run:', kwargs['data']

def print_some_times():
    time_to_sleep = 0
    interval_time = 60*8 # in seconds
    run_start = 22413

    start_time = time.time()

    for i in range(0, 518): # ending on 22931
        kwargs = {'data': '', 'output': '/home/whb43145/autoreduce_out'}
        kwargs['data'] = '/isis/Instruments$/NDXMERLIN/Instrument/data/cycle_14_1/MER' + str(run_start+i).zfill(5) + '.nxs'
        s.enter(i*interval_time, 1, call_reduce, (start_time, i, interval_time, kwargs))
    
    s.run()

print_some_times()

