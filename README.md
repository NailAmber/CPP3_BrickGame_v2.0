# BrickGame Snake
Summary: In this project, I implemented the Snake game in the C++ programming language in the object-oriented programming paradigm.

## Introduction

The project consists of two separate components to implement the Snake game: a library responsible for implementing the game logic and a desktop GUI.

The developed library must also be connected to the console interface of BrickGame v1.0. The console interface must fully support the new game.

The Tetris game developed in BrickGame v1.0 must be connected to the desktop interface developed in this project. It must fully support the game.

## Chapter I 
# General information

Remember to use finite-state machines to formalize the logic of the game.

### Snake

The player controls a snake that moves forward continuously. The player changes the direction of the snake by using the arrows. The goal of the game is to collect "apples" that appear on the playing field. The player must avoid hitting the walls of the playing field. After "eating" the next "apple", the length of the snake increases by one. The player wins when the snake reaches the maximum size (200 "pixels"). If the snake hits a boundary of the playing field, the player loses.

The game was based on another game called Blockage. It had two players controlling characters that left a trail that you couldn't run into. The player who lasted the longest won. In 1977, Atari released Worm, which was now a single player game. The most popular version of the game is probably the 1997 version released by the Swedish company Nokia for their Nokia 6110 phone, developed by Taneli Armanto.

### MVC Pattern

The Model-View-Controller (MVC) pattern is a scheme for dividing application modules into three separate macro components: a model that contains the business logic, a view - a form of user interface for interacting with the program, and a controller that modifies the model based on user actions.

The concept of MVC was described by Trygve Reenskaug in 1978 while working on the Smalltalk programming language at Xerox PARC. Steve Burbeck later implemented the pattern in Smalltalk-80. The final version of the MVC concept was not published until 1988 in Technology Object magazine. After that, the design pattern began to evolve. For example, a hierarchical version of HMVC was introduced; MVA, MVVM.

The main need for this pattern is related to the developers' desire to separate the program's business logic from the views, which allows them to easily replace views and reuse once-implemented logic in other contexts. A model that is separate from the view and a controller that interacts with it allows you to efficiently reuse or modify code that has already been written.

The model stores and accesses key data, performs operations defined by the program's business logic, and manages the part of the program responsible for all algorithms and information handling processes. These models, when modified by the controller, affect the display of information on the user interface view. The class library that implements the logic of the snake game must act as a model in this program. This library must provide all the necessary classes and methods to execute the game mechanics. This is the business logic of this program, as it provides the means to solve the problem.

The controller is a thin macro component that performs the modification of the model. All requests for model changes are generated through the controller. It looks like a kind of "facade" for the model in the code, that is, a set of methods that already work directly with the model. It is called thin because the ideal controller contains no additional logic other than calling one or more methods of the model. The controller acts as a link between the interface and the model. This allows the model to be completely encapsulated from the view. Such separation is useful because it allows the view code to know nothing about the model code and to refer only to the controller, whose interface of provided functions is unlikely to change significantly. The model, on the other hand, can change significantly, and when "moving" to other algorithms, technologies, or even programming languages in the model, only a small portion of the code in the controller directly related to the model needs to be changed. Otherwise, it is likely that much of the interface code would have to be rewritten, as it is highly dependent on the model implementation. Thus, when the user interacts with the interface, he or she calls controller methods that modify the model.

The view contains all the code associated with the program interface. There should be no business logic in the code of a perfect interface. It is just a form for user interaction.
