# arduino-alarm

A stupid-simple alarm system that can run on even an Arduino Nano. 

It features support for simple binary "security points" (aka their condition can either be "open" or "closed"), the most common being door and window reed switches. The connected speaker will beep (from slow to fast) if armed and a security point is triggered. It can be armed and unarmed to disable/enable aforementioned beeping like any other alarm system, but you must supply the password through Serial. 

Right now it is in its pretty basic, early stages, but it will be improved. 

# Documentation

## How do I add more "security points"?

Right now, it's a bit complicated but possible. 

First, add an integer variable to the `Security points` variable section. The value assigned is the digital pin the switch (be it a button, reed switch, sensor) is connected to. It must not be an already used pin. For example:

```
int window1 = 6;
```

Means the security point "window1" is connected to digital pin 6 `D6` and ground `GND`.

<hr>

Then, icrement `securityPoints`. Make sure it corresponds to the number of security points you have. For example: 

```
securityPoints = 3;
```

...since we have 3 security points.

<hr>

After that, add an array element to `isOpen`. Make sure that it is set to `false`. For example:

``` 
int isOpen[securityPoints] = {  
  false, false, false
};
``` 

Notice how there are now three array elements since we have 3 security points.

<hr>

Then, assign a `pinMode()` to your new security point by adding one under the `Security points` section under `void setup()`. For example: 

```
pinMode(window1, INPUT_PULLUP);
```

<hr>

Finally, create a code block for the logic of your security point under the `Security points` section under `void loop()`. Follow this template:

```
// <name>
if(digitalRead( <name> ) == <trig> && isOpen[ <point> ] == false) {
  Serial.println( <openMessage> );
  isOpen[ <point> ] = true;
  if(isArmed == true) soundAlarm = 1;
} else if(digitalRead( <name> ) == <otrig> && isOpen[ <point> ] == true) {
  isOpen[ <point> ] = false;
  soundAlarm = 0;
  Serial.println( <closeMessage> );
};
```

Ok, let's break this down;

  * **\<name>** is the name of your security point. *In this case it is "window1".*
  * **\<trig>** is the position of the switch that is **not normal**. For example, if you have a normally CLOSED switch, set it to HIGH, meaning it is triggered when the switch is OPEN. *In this case, it is "HIGH" because our window reed switch is normally CLOSED.*
  * **\<otrig>** is the **normal** position of the switch. *In this case, it is "LOW".*
  * **\<point>** is the element of the array that corresponds to your security point. *In this case, it is 2, the third element of the array, since "window1" is our third security point.*
  * **\<openMessage>** is the message to print through Serial when the switch is triggered. *In this case we'll make it "Window1 is now opened."*
  * **\<closeMessage>** is the message to print through Serial when the switch is un-triggered or returns to its normal position. *In this case we'll make it "Window1 is now closed."*

Here is an example using all the choices we made:

```
// window1
if(digitalRead( window1 ) == HIGH && isOpen[ 2 ] == false) {
  Serial.println( "Window1 is now open." );
  isOpen[ 2 ] = true;
  if(isArmed == true) soundAlarm = 1;
} else if(digitalRead( window1 ) == LOW && isOpen[ 2 ] == true) {
  isOpen[ 2 ] = false;
  soundAlarm = 0;
  Serial.println( "Window1 is now closed." );
};
```

# TODO: 

* Add a separate "until unarmed" alarm that fires after a warnLevel reaches maxWarnLevel
* Use a class-and-config based system for easier adding and editing security points.