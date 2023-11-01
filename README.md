# collection
A collection of projects I've worked on.

## Defender Bot

Defender Bot is a Reddit bot designed to engage in debates with Reddit users. It uses OpenAI's ChatGPT for generating responses. Managed using an SQLite database, The bot monitors its Reddit inbox, adds submissions to a "watchlist" for 3 days as requested, tracking token usage by user. It evaluates comments and decides whether they are argumentative or not. If so, it uses GPT-3 to generate a reply. The bot operates continuously and includes a logging system. The program has been containerized using Docker and hosted on the Google Cloud Platform.

- Built with PRAW, tiktoken, SQLAlchemy, and the OpenAI API.

## Musical Dominoes

![Musical Dominoes screenshot](https://github.com/Franckltung/collection/assets/58312008/c2f3a9aa-9c76-490a-90d6-56fb3249c190)

A Simple domino game with a musical twist. Designed to educate and train users how to understand and compare musical note and rest values. The program utilizes the SDL2 library for graphics and sound. It supports zooming and handles various game events, such as player victories and blocked moves. The architecture includes systems for input, rendering, sound, and game logic.
- Built with SDL2 and EnTT, a modern, high performance entity-component-system library.

## Parallel Boids
![Parallel Boids screenshot](https://github.com/Franckltung/collection/assets/58312008/93a97593-e7e7-4c14-94e5-0fb471b93bf4)

A parallel [Boid](https://en.wikipedia.org/wiki/Boids) simulator utilizing Intel Threading Building Blocks Flow Graphs to achieve parallel performance gains. The Boids follow 3 major rules:
- Separation
  
  Each boid tries to stay a certain distance from other boids. This prevents collisions.
- Alignment
  
  Each boid tries to match the average motion vector of its flock.
- Cohesion
  
  The boids attempt to fly towards the center of their flock.

The program was built using TBB and SDL2, and has a control panel built using ImGui to allow for live adjustment of simulation parameters.
