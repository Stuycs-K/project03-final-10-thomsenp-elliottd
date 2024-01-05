# Final Project Proposal

## Group Members:

Daniel Elliott, Preston Thomsen
       
# Intentions:

Simple kingdom builder, with a server acting "game engine", deciding on events for all clients and managing client interaction, and clients that show info based on text inputs, and pass commands back to server
    
# Intended usage:

Client end, a menu with different options that are shown as corresponding to keys(Ex: Stats: "s", Recruitment: "r", Buildings: "b") In each of these menus, numbers 1-9 will be used for suboptions, e.g. to recruit 100 troops might be "r" -> 3) On the server end, at the start will be a request for total player count and difficulty(as a multiplier of gold and population growth) as a terminal prompt, expecting integer format for both
  
# Technical Details:
Files shared between server and clients for storing stats, of talents, buildings, and troops, semaphores for turn managment and dueling, sockets will be used for communications between client and server, allocating memory will be used for storage of communications,and finally signals will allows us to handle errors and disconnceting.

Daniel Responsbilites: Set up server system, get events working

Preston Responsiblites: Set up file system with data, get abilties(methods) to read data for server and clients

Data structures as of now: file system for calling data(e.g. troop type number to cost, hp, dmg, event number to events), one server with numerous clients using select
    
# Intended pacing:

1/10/24: Finish server setup, have game structure working
1/13/24: Finish game(To point of playability)
1/16/24: Finish debugging, making quality of life improvements
