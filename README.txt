README:
simple guide for using the client:

The first prompt you receive allows you to choose between querying (q or Q), updating (u or U), cleaning (c or C) and simply exiting execution (e or E).
From each of these prompts (apart from exit) you are asked for an entry number.  This is the position in the array (+1).  So if you want the 0th element of the array, you enter 1.
From here, update is the only one with any prompts left.  It will ask you to enter a message, which you can enter \n into and it will replace it with the newline escape character.
Finally, you will be asked if you want to encrypt the message, though we didn't get around to implementing the cryptography, so all this will do is send "c" in place of "p" for the mode.

That's all you need to know!

NOTE:
client compiles with two warnings, this is because I'm reading %999[^\n] into a char array, which works fine, but the compiler issues warnings anyways.