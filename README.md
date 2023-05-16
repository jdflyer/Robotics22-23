# Robotics 2022-2023
This repository has the source code for my school's robotics team for the 2022-2023 school year using the [PROS](https://github.com/purduesigbots/pros) kernel.

While the project is (and will remain) unfinished, it can act as a good base for a text-based and easy to update menu system, as well as several drive systems for different robots.

In the project, there is also an autonomous system that can be used to record a series of inputs from the controller, save them to the SD card on the robot brain, and replay them either on demand or when a match starts.

To add code for another robot, search for one of the defined robots (eg. TEAM1), and copy that code for another robot. (#ifdef TEAM1 should become #ifdef TEAM2 under TEAM1's ifdef). Additionally, the EXTRA_CXXFLAGS variable in the makefile should be updated to define whatever robot is being used.