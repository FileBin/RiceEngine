# Rice Engine Net Protocol

## 1.  Packet scheme

  | Rice prefix   | Rice version      | Protocol version | Packet ID | Packet data |
  | ---           | ---               | ---              | ---       | ---         |
  | string        | (uint8, uint8)    | uint16           | uint16    | ...         |

  > * *Rice prefix:* string that indicates that packet is from Rice Engine. </br> *Default: "Rice"*
  > * *Rice version:* two bytes that indicates major and minor version of Engine.
  > * *Protocol version:* indicates version of protocol used.
  > * *Packet ID:* indicates what data is sent to client.
  > * *Packet data:* additional data that is sent to client.

## 2. Packet IDs

* ### ServerIdentification (0x00)

  *Usage:*
  > Response to a player when it needs to ident the server in server search panel.

  *Structure:*
  
    | Server name | Server MOTD |
    | ---         | ---         |
    | string      | string      |

* ### Ping (0x01)

  *Usage:*
  > Sent to clients periodically. The ping packet is used to determine if the connection is still open. 

* ### Dependency list begin (0x02)
  
  *Usage:*
  > Sent when client checks dependency code before join the server.

* ### Dependency list item (0x03)

  *Usage:*
  > Describes dependency.

  *Structure:*
  
    | Name | .DLL download link | .SO download link | Source code link |
    | ---  | ---                | ---               | ---              |
    | string | string           | string            | string           |

    > * *Name:* name of dependency.
    > * *.DLL download link* link to download compiled dynamic library for windows.
    > * *.SO download link* link to download compiled dynamic library for UNIX.
    > * *Source code link* link to source code to compile binary.

* ### Dependency list end (0x04)
  
  *Usage:*
  > Indicates end of dependency list.
