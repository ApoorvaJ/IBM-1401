# IBM-1401
A simple (and incomplete) emulator for the IBM 1401

This small program emulates a subset of the instruction set for the IBM 1401. It executes the machine language program `,008015,022029,036043,050054,055062,063065,069080/333/M0792502F1.065HELLO WORLD`, and prints out "HELLO WORLD".

I wrote this as a quick, Saturday morning recreational project, in collaboration with my dad ([Ramesh Joshi](http://www.consult400.com/myprofile.htm)), who began his programming career on this machine, in Philips in India in 1982.

You can see the hardware in all its glory in this [YouTube video](https://www.youtube.com/watch?v=PwftXqJu8hs), and read about the machine in its [Wikipedia article](https://en.wikipedia.org/wiki/IBM_1401).

Here's a pertinent quote from Wikipedia about the use of the machine in our part of the globe, long after it had been outdated in the western world:

> During the 1970s, IBM installed many 1401s in India and Pakistan where they were in use well into the 1980s. Some of today's Indian and Pakistani software entrepreneurs started on these 1401s. The first computer in Pakistan, for example, was a 1401 installed at Pakistan International Airlines.

###Program execution

The 1401 has a variable instruction length. The machine uses a variant of BCD as an encoding scheme, with support for alphabets and some special characters. In each byte, 6 bits are for data, one bit is for parity, and one bit is a word mark bits. Word mark bits are used to denote the beginnings of words.

In this emulator, I have used a simpler scheme for encoding that is more inline with the ASCII in modern computers. I have simply allocated two arrays of the 1401's full capacity of 16000 bytes, and used one for data and one for storing word marks. This makes it simpler to store and debug the data, and I can avoid the 1404<--->ASCII encoding. The rest of the booting and program execution process is described in the Wikipedia article.

This project could be a starting point for implementing the remaining instructon set of the machine if you are so inclined. Happy hacking. :)
