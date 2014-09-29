dcfpe
=====

Distributed Computation Framework for Project Euler

It is a tool to make computer compute together.


Brief Component List:
====================

Host:
* Discover workers from LAN.
* Discover workers from server. 
* Add workers manual.
* Comunnicate with Worker
* Maintain the H process and R process.
* Maintain the worker list. (View Workers, Network Quality, Worker ability(memory limit, process count limitation), Worker status(Memory usage, CPU usage, process count))
* Distribute task effectively, and manage them. Reliable.
* Receive tasks from H process and distribute them to W process. (H process -> Host -> Worker -> W process)
* Receive result from W process and send them to R process. (W process -> Worker -> Host -> R process)

Worker:
* Announce it is joining or leaving the LAN.
* Register it on the server.
* Communicate with Host.
* Maintain the W process.
* Receive tasks from Worker and pass them to W process.
* Receive result from W process and send them to Host.

H process:
* Generate tasks and send to Host

W process:
* Compute and send result to Worker

R process:
* Receive results from Host and combine them.

Other:
======

* Main language is C++11. It is possible to use python, lua, php, javascript.
* Basic network libary: ZMQ
* Worker will protect the W process. (sandbox)
* Trust mechanism between Host and Worker. (It is possible that the worker will receive a virus from Host).
* I do not when to release the first version because I am very lazy.

Development Environment(windows):
======
* win7 64
* python 2.7
* vs2013 or above (or win_toolchain 2013e in chromium)
* Build by vs2013
* Build step 1: make sure os.environ['GYP_GENERATORS'] = 'msvs' and os.environ['GYP_MSVS_VERSION'] = '2013'
* Build step 2: run command: python build.py
* Build step 3: open zmq_demo\zmq_demo.sln by vs2013
* Build step 4: build solution by vs2013
* Build by ninja with win_toolchain
* Build step 1: Config win_toolchain, modify src\build\win_toolchain.json
* Build step 2: make sure os.environ['GYP_GENERATORS'] = 'ninja' in build.py.(ignore os.environ['GYP_MSVS_VERSION'])
* Build step 3: run command: python build.py
* Build step 4: ninja -C output\Release



Updated:
========
* 2014.09.22 Upload gyp (basic project management tool) and chromium/base (basic code, especially the thread model).
