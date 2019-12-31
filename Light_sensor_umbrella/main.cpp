 // Continuously sweep the servo through it's full range
 #include "mbed.h"
 #include "Servo.h"
 
 void open_shade();
 void close_shade();
 int shade_mode();
 
 AnalogIn photocell(p15);
 AnalogIn photocell_1(p16);
 AnalogIn potentiometer(p17);
 DigitalIn switch_1(p18);
 DigitalIn switch_2(p19);
 DigitalIn pushbutton(p20);
 Servo myservo1(p21);
 
 
 bool shade_open = false;
 int main() {
     myservo1 = 0.0; // reset servo when program starts
     pushbutton.mode(PullDown); // put the button in pull down mode
     while(1) {
        int shade_function = shade_mode(); // find out which mode the shade is in (designated by switches)
        if (shade_function == 1) { // button controlled
            if (pushbutton) {   
                if (shade_open) { // if the shade is open close the shade
                    shade_open = false;
                    close_shade();
                } else { // if the shade is closed, open the shade
                    shade_open = true;
                    open_shade();    
                }
                wait(0.1); // wait to combat button debouncing
                while(pushbutton);
                wait(5.0); // cooldown for shade
            }    
        } else if (shade_function == 2) { //potentiometer angle controlled
            float pot_value = potentiometer.read(); // read the potentiometer value
            myservo1 = pot_value; // set the servo to the potentiometer value
        } else if (shade_function == 3) { // photocell sensor light sensing
            double average_light = (photocell + photocell_1) / 2; // find the average of the photocell values
            if (average_light > 0.8 && !shade_open) { // sunny out
                shade_open = true;
                open_shade();
                wait(5.0); // cool down
            } else if (average_light <=0.8 && shade_open) { // shadey out
                shade_open = false;
                close_shade();
                wait(5.0); // cool down
            }
        }
        
    }
}

// opens the shade by turning the servo
void open_shade() {
    for(float i = 0; i<1.9; i+=0.1) {
        myservo1 = i;
        wait(0.005); // cooldown between tick to make sure that the fan doesn't open too quickly
    }
}
// closes the shade by turning the servo
void close_shade() {
    for(float i = 1.9; i>=-0.1; i-=0.1) {
        myservo1 = i;
        wait(0.005); // cooldown between tick to make sure that the fan doesn't open too quickly
    }
}
int prev_mode = 0; // previous mode needed to compare to in order to reset the servo if the mode changes
int shade_mode() {
    if (!switch_1 && !switch_2) { // both switches off -> do nothing
        wait(0.5);
        return 0;    
    } else if (switch_1 && !switch_2) { // switch 1 on, switch 2 off -> button mode
        if (prev_mode != 1) {
            prev_mode = 1;
            shade_open = false;
            myservo1 = -0.1;
        }
        wait(0.5);
        return 1;
    } else if (!switch_1 && switch_2) { // switch 1 off, switch 2 on -> potentiometer mode
        if (prev_mode != 2) {
            prev_mode = 2;
            shade_open = false;
            myservo1 = -0.1;
        }
        wait(0.5);
        return 2;
    } else { // both switch 1 and 2 on -> light sensing mode
        if (prev_mode != 3) {
            prev_mode = 3;
            shade_open = false;
            myservo1 = -0.1;
        }
        wait(0.5);
        return 3;
    }
}
