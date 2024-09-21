# collection
A collection of projects I've built.

## Defender Bot

Defender Bot is a Reddit bot designed to engage in debates with Reddit users. It uses OpenAI's ChatGPT for generating responses. Managed using an SQLite database, The bot monitors its Reddit inbox, adds submissions to a "watchlist" for 3 days as requested, tracking token usage by user. It evaluates comments and decides whether they are argumentative or not. If so, it uses GPT-3 to generate a reply. The bot operates continuously and includes a logging system. The program has been containerized using Docker and hosted on the Google Cloud Platform.

- Built with PRAW, tiktoken, SQLAlchemy, and the OpenAI API.

## Musical Dominoes

![Musical Dominoes screenshot](https://github.com/Franckltung/collection/assets/58312008/24e4d9f3-da0d-4c8a-b4b8-8631ef019695)

A Simple domino game with a musical twist. Designed to educate and train users how to understand and compare musical note and rest values. The program utilizes the SDL2 library for graphics and sound. It supports zooming and handles various game events, such as player victories and blocked moves. The architecture includes systems for input, rendering, sound, and game logic.
- Built with SDL2 and EnTT, a modern, high performance entity-component-system library.

## Parallel Boids
![Parallel Boids screenshot](https://github.com/Franckltung/collection/assets/58312008/5f6996ab-286c-4baa-b6af-7636437e0424)

A parallel [Boid](https://en.wikipedia.org/wiki/Boids) simulator utilizing Intel Threading Building Blocks Flow Graphs to achieve parallel performance gains. The Boids follow 3 major rules:
- Separation
  
  Each boid tries to stay a certain distance from other boids. This prevents collisions.
- Alignment
  
  Each boid tries to match the average motion vector of its flock.
- Cohesion
  
  The boids attempt to fly towards the center of their flock.

The program was built using TBB and SDL2, and has a control panel built using ImGui to allow for live adjustment of simulation parameters.

## Profit

A command-line tool to identify mispriced trading goods in the MMO "Path of Exile" by interfacing with its HTTP API with Boost Beast. Uses RapidJSON to parse responses from the API. The program will automatically fetch all trading pairs, sort them in a table by price difference, and store the results in a file.

- This tools finds arbitrage opportunities by scanning for mispriced pairs of cards and items. Certain cards in the game can be converted into items once you have enough of them. If the total price of all the cards required to obtain an item is significantly higher or lower than the price of said item, this implies that one of the two is currently mispriced.
