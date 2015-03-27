﻿#summary Information for developers
#labels Section-Manual,Featured

# Developing Re-Mote #

This document is currently mostly a placeholder. It should be expanded as needed to cover important aspects related to the development of Re-Mote.

## Documentation and Recommended Reading ##

To get an understanding of the framework, [the initial project report](http://remote-testbed.googlecode.com/files/Zeuthen-2007.pdf) by Esben Zeuthen gives a good understanding of the design considerations. You should also get familiar with information in the AdministratorManual and UserManual and have the SecurityConsiderations in mind when touching parts of the code involving networking.

Javadoc for the [remote-ws](http://remote-testbed.googlecode.com/svn/documentation/remote-ws/index.html) and [remote-gui](http://remote-testbed.googlecode.com/svn/documentation/remote-gui/index.html) may serve as an introduction and reference for these components.

## Getting and Publishing the Source Code ##

Several repositories exists allowing you to choose your preferred way of getting the latest SourceCode. The service provided by http://repo.or.cz/ allows you to create and host your own "fork" of each of the DIKU repositories.

## Release Planning ##

The RoadMap contains the plan of future major milestones and uses the [issue tracker](http://code.google.com/p/remote-testbed/issues/list) to manage the different tasks. Additional [design documents](http://code.google.com/p/remote-testbed/w/list?can=2&q=label%3ASection-Proposal&sort=pagename&colspec=PageName+Summary+Type+Status+RevNum+ChangedBy+Changed) may exist from time to time, detailing enhancement proposals.

## The Build Process ##

Both of the above are used for generating WSDL files describing
the services made available. Using the WSDL files, client Java classes
for accessing the webservices can then be created and compiled for use
by the Re-Mote client.

  * Something about the Makefile rules for deploying. Saying that they are best for localhost?

## Testing the Deployment ##

The web services can be tested:

  * Using SOAPMonitor, see contrib/soapmonitor/ in `remote-ws`.
  * Using tcpmon