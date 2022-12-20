# Canadian-Rescue-Assistive-Prototype-Robot

Final Project for the 1A Mechatronics Engineering Term at the University of Waterloo. 
![Untitled design (4)](https://user-images.githubusercontent.com/71574223/202064548-eb99529f-b643-4678-8e70-7aa5af253d09.png)

We are group 4-18. Our members include [Kevin Wang](https://github.com/kevinjunkaiwang), [Vedha Mereddy](https://github.com/vedhamereddy), Janet Pham, and [Mckale Chung](https://github.com/mckalechung). 

# Purpose
With the susceptibility of individuals to get lost in harsh, isolated environments with extreme negative temperatures, rescue missions must be accurate, efficient, and safe. However, sending a rescue team to save these victims comes with severe risks due to the dangers and challenges of such environments. There is also a high probability that the mission will fail and put more lives at stake. 

Consequently, to automate the search and rescue process and limit human intervention, team 4-18 developed the Canadian Rescue Assistance Prototype (CRAP)©. CRAP is a robot that can travel through the relatively flat, isolated proximity of a base camp in freezing temperatures, locate injured individuals at risk of danger and safely return them to the camp while providing medical professionals with essential reports regarding their exposure to the temperature.

# Software and Build
The robot is created using a LEGO EV3 Brick, and programmed in RobotC. Given an input file detailing the location and information of a person, it chooses where to find that person. It then travels a boustrophedon to find them, avoiding obstacles along the way.

Once the robot finds a person, it gently sweeps them in, rotates to face the x-axis, and returns to the x-axis. It then turns toward the center and returns to safely drop off the person. Afterward, it outputs the time of exposure, in an output file. 

If the robot does not find a person, it travels to the end of the boustrophdon then returns back, outputting that it could not find the patient. 

Although there are different files for each quadrant, each contains the same information. We had to seperate them to write 4 distinct output reports. Otherwise, each file is the same, and has the same decisions and algorithms to go to any quadrant, depending on what the input file instructs. 



