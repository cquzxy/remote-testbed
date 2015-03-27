# Archive of Development Roadmaps #

Below roadmaps for previous releases are available starting with the newest.

## Re-Mote Testbed Framework version 1.0 ##

The overall goal of this release is to have all the basic things working. The main focus is to make the "backbone" of the framework support features required by the DIKU and CIT testbeds.

Issues sorted by [status](http://code.google.com/p/remote-testbed/issues/list?q=label:milestone-release1.0&can=2&x=component&y=status&mode=grid) or [priority](http://code.google.com/p/remote-testbed/issues/list?q=label:milestone-release1.0&can=2&x=component&y=priority&mode=grid). [All](http://code.google.com/p/remote-testbed/issues/list?q=label:milestone-release1.0&can=1) issues belonging to this milestone.

### Usability ###

Not a lot will change on the client side, but it should be polished a bit, especially regarding startup, and unfinished features should be hidden from the user. Documentation for administrators should be provided to ease installation and maintainence of testbeds.

  * [Issue 3](https://code.google.com/p/remote-testbed/issues/detail?id=3): Display mote platform in the mote panes.

  * [Issue 13](https://code.google.com/p/remote-testbed/issues/detail?id=13): Listing supported platform and required dependencies are a must for new people considering the use of Re-Mote.

  * [Issue 14](https://code.google.com/p/remote-testbed/issues/detail?id=14): Remove/disable the potentially misleading "motes map", since it is unlikely to be ready for this release.

  * [Issue 28](https://code.google.com/p/remote-testbed/issues/detail?id=28): Rename TOSADDRESS to NETADDRESS

### Security ###

The focus for this release is not security and no extensive code audit has been made, but since this release is intended to change the status of the framework from "testing" to "production", problem areas that have been uncovered in the release cycle need to be fixed.

  * [Issue 1](https://code.google.com/p/remote-testbed/issues/detail?id=1): Prevent possible SQL injection attacks. All queries should be changed to use `PreparedStatement`.

### Interoperability ###

The framework will be running at multiple sites making requirements for interoperability and portability more important. Although big changes are not expected, improvement to make it conform more to the design of existing systems will make it easier to port to new future sites.

  * [Issue 2](https://code.google.com/p/remote-testbed/issues/detail?id=2): Fix problems with mysql++ usage being incompatible across different versions of the library.

  * [Issue 5](https://code.google.com/p/remote-testbed/issues/detail?id=5): Prepare DB for IPv6 support.

  * [Issue 18](https://code.google.com/p/remote-testbed/issues/detail?id=18): Make MCI daemons create .pid files in /var/run/.

  * [Issue 20](https://code.google.com/p/remote-testbed/issues/detail?id=20): Move UDEV helper scripts from /etc/udev/scripts/ to /lib/udev.

### Infrastructure ###

Changes to the "backbone" infrastructure is the main focus. The plan is to make especially the very site specific parts of MCI more generally usable and easy to adapt to new testbed infrastructures.

  * [Issue 6](https://code.google.com/p/remote-testbed/issues/detail?id=6): Make MCH support the use of external programs to control mote start, stop, and reset, similar to how programming is currently done via an external program.

  * [Issue 21](https://code.google.com/p/remote-testbed/issues/detail?id=21): Configure path to mote flash programmer via device hierarchy.

  * [Issue 24](https://code.google.com/p/remote-testbed/issues/detail?id=24): Place temporary mote program file in device hierarchy.

  * [Issue 7](https://code.google.com/p/remote-testbed/issues/detail?id=7): Add support for Tmote and MicaZ.

  * [Issue 9](https://code.google.com/p/remote-testbed/issues/detail?id=9): Using strings for mote TOS/MAC addresses will allow to simplify things considerably and remove usage of the `tosaddress` and `macaddress` DB tables.

  * [Issue 10](https://code.google.com/p/remote-testbed/issues/detail?id=10): Remove the `tosaddress` and `macaddress` site specific tables.

  * [Issue 15](https://code.google.com/p/remote-testbed/issues/detail?id=15): Reorganizing the GUI code will make it more hackable in Java IDEs, which are very good at catching stuff like deprecated methods.

### Performance ###

  * [Issue 29](https://code.google.com/p/remote-testbed/issues/detail?id=29): Make use of a new BSL for TMote Sky & TelosB platform written in C++