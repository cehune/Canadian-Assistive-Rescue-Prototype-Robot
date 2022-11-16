# Canadian-Assistive-Rescue-Prototype-Robot
Final Project for the 1A Mechatronics Engineering Term at the University of Waterloo. This is a 1.5 month long project with both mechanical and software components. This repository covers the project, its design, and its objective. 
![Untitled design (4)](https://user-images.githubusercontent.com/71574223/202064548-eb99529f-b643-4678-8e70-7aa5af253d09.png)

We are group 4-18. Our members include Kevin Wang, Vedha Mereddy, Janet Pham, and Mckale Chung. 

#Purpose
It is easy for individuals to get lost in harsh, isolated environments with extreme negative temperatures. 

However, sending other people to save these individuals comes with heavy risk due to the dangers and challenges of those particular environments and the high probability for the mission to fail and put more lives at risk. 

The Canadian Assistive Rescue Prototype automates the search and rescue process. It simulates this on a playing field with four quadrants, and a home base at the middle. We give the robot a file documenting the people it must save, their location, and how long they have been exposed to the cold environment. It then travels a boustrophedon, avoiding obstacles, until it finds the person it needs to save. It then returns them back to the center, drops them off, and moves on to save the next person.  

#Mechanical Design
This robot is made of Lego Ev3 Components, and programmed with RobotC. 

The main design component is the sweeping mechanism at the front. It opens via a medium EV3 motor, and closes to secure the person in place. 

We use 4 main sensors
1. The IR sensor identifies people based on distace.
2. The ultrasonic sensor identifies obstacles based on distance. We differentiate the two by making obstacles taller than people. Therefore, if the ultrasonic sensor activates, then we know its an obstacle. 
3. The color sensor identifies the center of the field, marked by black electrical tape. 
4. The gyro enables the robot to track where it's facing, and use this to return to the center no matter where it picks up a person. 




