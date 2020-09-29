# Logging Library-
Simple Logging library for C++


## Problem Definition
Problem is about writing a logging library in preferred language. The program should contain log
levels (DEBUG, INFO, WARNING, ERROR in this order. Log () , getErrors(), clear() functions
should be provided. Log entries should have unique IDs. It also should contain some
configurable and externally configurable settings. (described in task).


## Solution Design
I have tried to implement everything in header only file. So the other person can easily include
header file and can use logging library. Also if we provide a header only library then all the code
gets compiled with the same compiler settings and against the same headers so a lot of problems
go away ( so we'll not end up with error reports on machines and with compilers you've never heard of). I
have used singleton design pattern for header because we only need one object from this class. I
have also used serialization for making service externally configurable.

## Authors

* **Subhan Hagverdiyev** 


## Acknowledgments
This is the project prepared as Nokia R&D Software Engineer Trainee position task.



