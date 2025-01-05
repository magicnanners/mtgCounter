# mtgCounter
Arduino based MTG life counter

# About
This is an arduino based electronic life counter for MTG Commander/EDH. This is my first coding project in a number of years so you're likely to find some oddities within it. It is designed around an Arduino Nano and an SSD1306 128X32 OLED display utilizing the Adafruit GFX and SSD1306 libraries. The user experience is currently pretty clunky with only 4 buttons, I plan to implement a more traditional D-Pad for navigation but I currently only have 5 tac switches and wanted to at least complete the proof of concept. In the short term I will be adding a 5th button to be able to cycle left through the current row. 
# I want to build this, what will I need?
You'll need the following:  
-1X Arduino Nano Note: An Uno board will work just as well and shouldn't require modification. I used a Nano for convienence.  
-1X SSD1306 OLED Display  
-4X Buttons Note: Can really be any buttons, I used standard tac switches but you can use whatever you like, the world is your oyster.  

# Build Guide
TODO: Add wiring diagram.  
# Photos
TODO: Add photos  
# Future Plans
Currently the code only supports a standard 4 player game of Commander with a starting life total of 40. In the future I plan to implement a setup screen where you can select number of players and starting life total.
The code currently can track when a game has ended and displays a game over screen however it does not currently feature a quick way to reset. For now you'll have to press the reset button on your Arduino to reset the game.  
Once this version is feature complete I plan to design a 4 player variant, much of the code from this project will be reused in that project with some added goodies and greeblies.  
# Contributions
Please feel free to add contributions to this code and provided they fit within the scope of what I'm trying to accomplish I'll be more than happy to review the request. Or you can always fork it, that's cool too.
# Thanks
Thanks for checking out my project, as mentioned earlier this is my first major coding project in a number of years, it's been something floating in my head since early 2024 so it's nice to see it finally realized. 
  
