﻿#summary Security considerations for administrators
#labels Section-Technical

# Security Considerations #

This page tries to give an overview of security issues related to the various components and explain what can or should be done.

## The Running System ##

The framework requires that 3 ports are open for incoming connections:

  1. The web service port (usually 8080).
  1. The mote control server client port (default 10000) listening for new client connections.
  1. The mote control server host port (default 10001) used by joining mote hosts.

The web services should be considered public in that no
restrictions are put on who gets access to the returned
pages. Apart from the authentication service, they do,
however, depend on a valid session ID. The same is true
for the client port, which requires that the client has
been authenticated before accepting any requests. Finally,
only mote hosts that connects from preapproved IP addresses
are allowed to join the mote control infrastructure.

## The Protocols ##

### Transfer of Sensitive Information ###

Data between the client and web services are sent as clear text. This may pose problems,  if credentials used in the testbed are system wide for the implicated users.

## The Server Code ##

In its current shape, the server code should not be considered as
secure as desirable. It has been written with robustness in mind,
however, there is still room for improvements.

## Fixed and Open Security Issues ##

Below, an overview of the security related issues recorded in the issue tracker is listed:

  * [Issue 1](https://code.google.com/p/remote-testbed/issues/detail?id=1): User derived input in the web services was not handled properly.

Search the issue tracker for [issues marked as security related](http://code.google.com/p/remote-testbed/issues/list?can=1&q=label:Security).