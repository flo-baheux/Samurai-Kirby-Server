# Samurai-Kirby-Server
Dedicated game server for the game Samurai Kirby [available on itch.io](https://laywelin.itch.io/samurai-kirby) (password Laywelin-42!)

## What is Samurai Kirby
Samurai Kirby is a two players reflex game where the first to press the *correct* button wins!
It is a more-than-obvious homage to the Samurai Kirby mini-game from Kirby Super Star on SNES in 1996.

It's exclusively online using a dedicated server, and the game was made in 3 months as my first Unity project (and the server is my first C++ project). 
Server is hosted on AWS on an EC2 T2-Micro instance.

You can find the sources here!

This project is done in the context of my portfolio. As a senior backend engineer with 7 years of experience, 
I want to dedicate my career to working in the videogame industry. I'm now an aspiring gamedev / network engineer.

## The server
The server itself was made using C++. Coming from Typescript I did not use C++ since college, so 10 years ago.
Working on this project I wanted to learn few things: 
- smart pointers
- list initialization (yes it was not a thing back then)
- basic threading
- TCP communication
- dependency injection in C++
- testing in C++
- Serialization / Deserialization in C++

The server uses a strong separation between network and gameplay.
Network is running in the main thread while Gameplay is in a separate one. Both communicate via messages using a pub-sub mechanism [add link to file].
[add schema].

There is a very low risk of thread issues I'm aware of.
I mitigated this by using unordered_maps<PlayerId, Queue<Message>> and a different queue from client to server, and server to client.
[Elaborate!!!] 

Tests are done using [Catch2](https://github.com/catchorg/Catch2), and Serialization using [nlohmann/json](https://github.com/nlohmann/json)

## The problem I wanted to solve with server gameplay

The initial goal of this project was to build a proof of concept around how to solve the issue of
finding who's the first player to press a button in a fair way, given each player has network latency
<iframe style="border:none" width="800" height="450" src="https://whimsical.com/embed/Pcbb2txmJaxonVzuMHzw63@6HYTAunKLgTUcu2PgNLnYmspAaJUSFePcXbK3AePVYE8aQs"></iframe>

I found out there is many solutions to this problem, and decided to implement that one:
<iframe style="border:none" width="800" height="450" src="https://whimsical.com/embed/Pcbb2txmJaxonVzuMHzw63@or4CdLRbgixaTuPNLRMgaFjwELG8TkrMxoPuu8Drs"></iframe>

I did not implement anticheat, because the scope of the game became a bit too large.

However the two possibilities I had in mind were as follow.
- Logic based: if the player sends a reaction time that seems very unlikely for a human (say between 0 and 100ms) the input is rejected.
- Encryption: sending encrypted payloads to mitigate possibilities of man-in-the-middle script. As the client is shipped key could eventually be found by source analysis but it's still a mitigation.

Another solution would have been to only ask clients for an input, and server detects client latency and takes it into account when determining who won.

## Communication contract

Using TCP the client and server are communicating via messages.
Client to Server is a PlayerAction, server to client is a GameplayMessage.

[Add schema]


## Things I wish I had time for / I would like to add

- More tests, especially around threading
- Proper Logging
- Containerization
- Basic Monitoring
- 



