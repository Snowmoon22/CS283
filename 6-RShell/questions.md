1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

_The remote client determines when a command's output is fully received by looking for an EOF character (RDSH_EOF_CHAR, 0x04). A way to handle partial read could be to send the length of the message before the actual data, so the client knows how much data to expect. Or the client can use a loop to repeatedly call recv() until the entire message is received._
 
2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

_ Since TCP is a  stream protocol, our shell protocol must define its own message boundaries. This can be done with delimiters, such as EOF which mark the end of command. Without proper framing, problem like message fragmentation, where the command is split across multiple packets could happen._

3. Describe the general differences between stateful and stateless protocols.

_Stateful Protocols maintain information about the state of the connection between the client and server whereas stateless protocols doesn't. Stateful is easier to implement complex interactions where server need to remember context of interactions._

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

_UDP is preferred when speed and simplicity are more important than reliability._

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

_The Socket API abstracts the underlying network protocols (e.g., TCP, UDP) and provides a consistent interface for applications to communicate over a network._