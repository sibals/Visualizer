Project 3 for CS559
by Emanuel Rosu and Jackson Reed
--------------------------------------

Project BARS: A simple music visualizer that changes dynamically to any song loaded into it

Controls:
- Q to toggle between post-processed background and screen space shader
- PgUp/PgDown to speed up/slow down the action
- P to pause
- Esc to quit

Features:
- A dynamic music visualizer rendered in the back ground using a post-processing method
- Very long, transparent ribbons formed from splines
- Spheres with different shader examples: 
	- Model space texture: based on the "energy" from the music, the bouncing spheres change color
	-  Eye space texture: the sphere in the front displays a texture created from its position within a 3D tecture
	- Post process texture: The sphere in front of the blue planet gives a raindrop effect by rendering the background onto it
	- multi-light ADS shader: The blue planet has two specular highlights
	- Screen space shader by pressing Q, changes based on screen position