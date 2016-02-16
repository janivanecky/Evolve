# Evolve

This is a project to try out the optimization by evolutionary algorithm. 

##What's the task to optimize?

![alt text](https://github.com/janivanecky/Evolve/blob/master/Win32RTRT/img/img1.png "Evolutionary red circle")

* Everything happens in the red circle
* 10 cannons (green dots in the picture) are randomly placed (but consistently across generations) within the red circle
* Each cannon shoots a ball (gray dots in the picture) that happens to fly near it
* Each cannon has a parameter that defines the direction in which it shoots the nearby ball
* Ball is accelerated after being shot from the cannon, but slows to a constant velocity in short time
* If the ball hits the wall of the red circle it bounces back unless it has sufficient velocity (provided by a cannon) to escape 

##What's being optimized?

* Parameters specifying direction for each cannon are modified by evolutionary algorithm
* Goal is to get all the balls from the red circle as fast as possible

##How it's done?

* Each generation contains 50 different "organisms" (cannons with different directions)
* Next generation is selected from 10 fittest organisms from previous generation with random mutation applied to them
* Best current solution is being tracked and if it doesn't change for 20 generations, mutation rate is increased.

##How about some results?

All the balls are out in 11.6 seconds
![alt text](https://github.com/janivanecky/Evolve/blob/master/Win32RTRT/img/ev.gif "Evolutionary red circle")

This one is really fast, 5.6 seconds to get all the balls out.
![alt text](https://github.com/janivanecky/Evolve/blob/master/Win32RTRT/img/ev2.gif "Evolutionary red circle")


