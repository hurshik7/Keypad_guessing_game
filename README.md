# Keypad_guessing_game

# Purpose

This project is a guessing game using two Raspberry Pis, a keypad, LCD, LED, and reliable UDP. The guessing game is that the server randomly generates a number from 0 to 9999 and a client matches the number within a certain number of times. On each attempt, the client receives a hint about the number from the server based on the number previously guessed. For example, if the server's random number is 8888 and the client enters 5000 on the first try, the client will get a LOW as a hint from the server. That means 5000 is lower than the random number. The purposes of each server and client are below:



* Client
1. Connects to game server
2. Receives a number from the user on the keypad and transmits the number to the server
3. Receive guess results from the server and print them on the LCD
4. Repeat process 2-3 for a specific number of times allowed by the server.
* Server 
1. Wait for client to connect
2. Takes a number from the client and sends a hint back to the client
3. Display the status of this server using LCD and LED.
4. Wait for another client when the client finishes playing.


# Intended Users

This guide is intended for users who have basic experience with the POSIX system, Raspberry Pi, and also some knowledge about breadboards, electronic sensors, and resistors.



* Users who can run programs using a terminal on a POSIX system
* Users who know what the command line arguments and flags are
* Users who know how to use the cmake
* Users who know how to build a circuit with a breadboard and sensors.


# Software Requirements



* Clang-tidy
* Cmake
* Dot (This is used by Doxygen)
* Doxygen (Although this is not necessary to run the program, it is intended to be used when building this program, so if you want to build without creating a Doxygen file, edit CMakeLists.txt)
* Standard C17 compiler (e.g. GCC, Clang)
* wiringPi


# Platform



* Raspberry Pi OS (Raspbian)


# Installing


## Obtaining

Clone this repo.

```
git clone https://github.com/hurshik7/Keypad_guessing_game.git
```


Then, you can see the `source` directory.


## Prerequisite Software


### wiringPi

You will need to install the WiringPi library, which provides a set of C functions to interact with the keypad, LCD, and LED needed to run this project. Follow the commands below if you haven’t done this before.


```
sudo apt-get update
git clone https://github.com/WiringPi/WiringPi
cd WiringPi
./build
```



## Building and Installing


### Hardware

Build the circuit for the client and server like below.


#### Server

The circuit for the server is as follows. You will need a LCD1602, a RGB Led, and three 220 Ohm(Ω) resistors.


![alt text](https://github.com/hurshik7/Keypad_guessing_game/blob/main/img/server1.png?raw=true)

_Figure 1. Server circuit_

<p id="gdcalert2" ><span style="color: red; font-weight: bold">>>>>>  gd2md-html alert: inline image link here (to images/image2.png). Store image on your image server and adjust path/filename/extension if necessary. </span><br>(<a href="#">Back to top</a>)(<a href="#gdcalert3">Next alert</a>)<br><span style="color: red; font-weight: bold">>>>>> </span></p>


![alt_text](images/image2.png "image_tooltip")


_Figure 2. LCD1602. (“SunFounder Da Vinci Kit for Raspberry Pi — SunFounder davinci-kit-for-raspberry-pi documentation”)_

<p id="gdcalert3" ><span style="color: red; font-weight: bold">>>>>>  gd2md-html alert: inline image link here (to images/image3.png). Store image on your image server and adjust path/filename/extension if necessary. </span><br>(<a href="#">Back to top</a>)(<a href="#gdcalert4">Next alert</a>)<br><span style="color: red; font-weight: bold">>>>>> </span></p>


![alt_text](images/image3.png "image_tooltip")


_Figure 3. RGB Led. (“SunFounder Da Vinci Kit for Raspberry Pi — SunFounder davinci-kit-for-raspberry-pi documentation”)_


#### Client

The circuit for the client is as follows. You will also need an LCD1602, a keypad, and 10KΩ  resistors. Connect the LCD1602 to the GPIO Extension board in the same way as the server.



<p id="gdcalert4" ><span style="color: red; font-weight: bold">>>>>>  gd2md-html alert: inline image link here (to images/image4.png). Store image on your image server and adjust path/filename/extension if necessary. </span><br>(<a href="#">Back to top</a>)(<a href="#gdcalert5">Next alert</a>)<br><span style="color: red; font-weight: bold">>>>>> </span></p>


![alt_text](images/image4.png "image_tooltip")


_Figure 4. Client circuit_



<p id="gdcalert5" ><span style="color: red; font-weight: bold">>>>>>  gd2md-html alert: inline image link here (to images/image5.png). Store image on your image server and adjust path/filename/extension if necessary. </span><br>(<a href="#">Back to top</a>)(<a href="#gdcalert6">Next alert</a>)<br><span style="color: red; font-weight: bold">>>>>> </span></p>


![alt_text](images/image5.png "image_tooltip")


_Figure 5. Keypad. (“SunFounder Da Vinci Kit for Raspberry Pi — SunFounder davinci-kit-for-raspberry-pi documentation”)_


### Software

First, go into the `source/guessing_client/` directory and build the client with the following commands.


```
cd source/guessing_client
cmake -S . -B build
cmake --build build
```


Second, go back to the `source/` directory and go into the `guessing_server/` directory to build the server.


```
cd ..
cd guessing_server
cmake -S . -B build
cmake --build build 
```


Note: The compiler can be specified by passing one of the following to cmake when configure the project:



* `-DCMAKE_C_COMPILER="gcc"`
* `-DCMAKE_C_COMPILER="clang"`

(For example, `cmake -S . -B build -DCMAKE_C_COMPILER="gcc")`


## Running


### Server

 You are able to configure the ip address and port number of the server. The default port number is `5050` if you do not input the argument. These are the examples of commands to run the server. Let’s assume the current directory is `source/guessing_server/` when running the server.

	`./build/guessing_server`


```
	./build/guessing_server -i <ip address>
	./build/guessing_server -p <port number>
	./build/guessing_server -i <ip address> -p <port number>
```



### Client

You are able to configure the IP address and port of the **server**. Let's assume the current directory is `source/guessing_client/` when running the client. If you do not input the port number with the -p flag, the default port number for the server is `5050`. (The ip address of the proxy server must be provided with -o flag)

	`./build/guessing_client -o 192.168.0.1`


```
	./build/guessing_client -o 192.168.0.1 -p 5000
```



# How To Play


## Run the programs

First, both server and client can check the current status through LCD. Run the client while the server is running first. Even if the server is down, the client will continue to search for the server. The following is the LCD screen you can see on each device.



* Server



<p id="gdcalert6" ><span style="color: red; font-weight: bold">>>>>>  gd2md-html alert: inline image link here (to images/image6.jpg). Store image on your image server and adjust path/filename/extension if necessary. </span><br>(<a href="#">Back to top</a>)(<a href="#gdcalert7">Next alert</a>)<br><span style="color: red; font-weight: bold">>>>>> </span></p>


![alt_text](images/image6.jpg "image_tooltip")
 

(waiting a client)



<p id="gdcalert7" ><span style="color: red; font-weight: bold">>>>>>  gd2md-html alert: inline image link here (to images/image7.jpg). Store image on your image server and adjust path/filename/extension if necessary. </span><br>(<a href="#">Back to top</a>)(<a href="#gdcalert8">Next alert</a>)<br><span style="color: red; font-weight: bold">>>>>> </span></p>


![alt_text](images/image7.jpg "image_tooltip")
 

(when a client connects to this server)



* Client



<p id="gdcalert8" ><span style="color: red; font-weight: bold">>>>>>  gd2md-html alert: inline image link here (to images/image8.jpg). Store image on your image server and adjust path/filename/extension if necessary. </span><br>(<a href="#">Back to top</a>)(<a href="#gdcalert9">Next alert</a>)<br><span style="color: red; font-weight: bold">>>>>> </span></p>


![alt_text](images/image8.jpg "image_tooltip")


(When the first data communication to the server is successful, which means when connected to the server)

(we don’t want to use the term “connect” since we are using reliable UDP not TCP)



<p id="gdcalert9" ><span style="color: red; font-weight: bold">>>>>>  gd2md-html alert: inline image link here (to images/image9.jpg). Store image on your image server and adjust path/filename/extension if necessary. </span><br>(<a href="#">Back to top</a>)(<a href="#gdcalert10">Next alert</a>)<br><span style="color: red; font-weight: bold">>>>>> </span></p>


![alt_text](images/image9.jpg "image_tooltip")


(when the game starts)


## Play Guessing Game

When the game starts, on the client side you can enter numbers via the keypad. Enter a 4 digit number and it will automatically be sent to the server and you will receive a hint from the server based on the number you sent. And you will also receive the number of remaining attempts to guess from the server, which will be displayed on the LCD as Life. Now, guess the target number! Good luck.


# Features


## Server



* All socket communication uses reliable UDP.
* When you run the server you can see a message on the console with the port number. The default port number is `5050`.

    ```
    Listening on port 5050
    ```


* When the server is running, it waits for a client to try to connect, and when it connects with one client, it starts the game.
* When the game starts, it generates a random number and receives a guessing number for this number from the client.
* Based on the client's guess number, it sends a hint to the target number and the number of lives remaining to the client. The hints this server sends are:
    * HIGH: “Your number is higher than the random number”
    * LOW: “Your number is lower than the random number”
    * CORRECT: “You got it!”
* When the game with a client ends, the settings of the game(the number of lives and the random number) are initialised and wait for a connection of another client. 


## Client



* All socket communication uses Reliable UDP.
* When you run the client you can configure the ip address and port number of the server. The default port number for the proxy server is `5050`.

    ```
    ./guessing_client -o 10.0.0.16 -p 5050
    ```


* When the game starts, it tries to connect to the server, and when connected to the server, the OK sign appears on the LCD.
* Play the game by entering numbers using the keypad.
* When a hint is received from the server, the hint and the number of lives remaining are printed on the LCD.
* When you win or lose the game, the results are displayed on the LCD.

# References



1. Circuit images are generated by Fritzing.

_Welcome to Fritzing_, https://fritzing.org/. Accessed 28 November 2022.



2. Circuits are designed by SunFounder.

    “SunFounder Da Vinci Kit for Raspberry Pi — SunFounder davinci-kit-for-raspberry-pi documentation.” _SunFounder's Documentations!_, https://docs.sunfounder.com/projects/davinci-kit. Accessed 28 November 2022.
