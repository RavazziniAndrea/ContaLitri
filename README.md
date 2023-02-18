# ContaLitri
### To show the people how much they have drunk

[![License](https://img.shields.io/badge/License-Apache_2.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)

[![](https://img.shields.io/badge/Hackster-2E9FE6.svg?style=for-the-badge&logo=Hackster&logoColor=white)]()

Brief story behind the project.
In my little town there's an annual festival to celebrate grape in every way. Long story short, it's an excuse to drink a lot of good wine. But how much? How many liters?
To answer this questions, I came up with the solution shown in this project. 

There is a server hosted on a Raspberry Pi 3b (the Java part) and an ESP32 and ~5m of led strips, to visualize the total liters of alcohol drunk.
The server:
1. Reads the data from a Postgres database (written by a third party software used to manage orders)
2. Calculate the total amount of liters drunk from the data
3. Expose the grand total that can be accessed with a simple GET request

On the client side, there is an ESP32 board that reads from the server and drive 4 addressable led strips (one for each digit from 0000 to 9999) to visualize the total amount of liters drunk.


[![](https://img.shields.io/badge/LinkedIn-0077B5?style=for-the-badge&logo=linkedin&logoColor=white)](https://www.linkedin.com/in/andrea-ravazzini/)
[![Open Source](https://badges.frapsoft.com/os/v1/open-source.svg?v=103)](https://opensource.org/)
