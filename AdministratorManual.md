﻿#summary How to install and maintain a testbeds with Re-Mote
#labels Section-Manual,Featured

# Re-Mote Testbed Administration #

This manual for administrators has instructions on how to install and configure a testbed as well as information about maintaining a running testbed.

**Table of contents:**




---


# An Overview of the Testbed Framework #

The Re-Mote Testbed Framework consists of four components:

_Database Scripts_ (`remote-db`)

> Contains SQL scripts for creating and maintaining the MySQL database. It is used by the web services and the mote control infrastructure for tracking the state--static as well as dynamic--of the testbed. The underlying data model is based on the three fundamental entities: motes, mote host sites, and user sessions.

_Web Services_ (`remote-ws`)

> Defines Java-classes that provide web services, which allow clients to interact with the testbed. The web services are responsible for authenticating users, granting access to motes, and getting mote specific information, such as availability and MAC address.

_Mote Control Infrastructure_ (`remote-mci`)

> Provides the infrastructure that keeps track of which motes have been deployed and where. It is divided into two parts: the _mote control host_, which is responsible for the low-level interaction with the motes in the testbed, and the _mote control server_, which serves mainly as message forwarder between mote hosts and connected clients. Written in C++ for performance.

_GUI Client_ (`remote-gui`)

> Implements client Java-libraries for interacting both with the web services and the mote control server and a GUI client allowing users to connect to a testbed, program motes, and monitor console output from motes. This component is the only one that does not require any site specific customization.

To give an idea of how everything is connected the following figure gives an overview:

![http://remote-testbed.googlecode.com/svn/documentation/images/WSNTestbedDeri.png](http://remote-testbed.googlecode.com/svn/documentation/images/WSNTestbedDeri.png)


---


# Before Deploying the Testbed #

Before deploying, you are advised to first read the SecurityConsiderations. It lists information about potential security threads, including what ports needs to be open on the server.

## Library and Program Dependencies ##

All components requires that a POSIX compliant shell (e.g. `bash`) and GNU `make` are installed on the system. For building and running the web services a Java runtime and compiler must be available. The mote control infrastructure requires that both a C and C++ compiler (e.g. gcc) has been installed. Note that for the C and C++ libraries listed below, development files such as header files are often packaged separately. For each dependency, a tested and "known-good" minimum version is listed. This is only informational and other versions may also be supported.

The server-side (mote control server, database etc.) build and runtime dependencies are listed in the following table:

| **Component**  | **Dependency**                                   | **Version** |
|:---------------|:-------------------------------------------------|:------------|
| `remote-db`  | MySQL server and client program and utilities  | - |
| `remote-mci` | MySQL C library                                | >= 5.0.45 |
|              | MySQL C++ library                              | >= 2.0.7 |
|              | Boost program options library                  | >= 1.34.1 |
| `remote-ws`  | Apache Tomcat                                  | >= 5.5 |
|              | Axis                                           | >= 1.4 |
|              | MySQL JDBC Connector Java library              | >= 5.0.4 |


The mote control host depends on udev (>= 113) and the boost program options library (>= 1.34.1).

If you are building `remote-mci` from an SVN or git checkout you will also need autoconf (>= 2.59) and automake (>= 1.4) to generate the `configure` script and `Makefile`s.

## Setting up Tomcat and Axis ##

  1. Start out by copying the `axis/` example directory from the axis-bin-1.4 release to the $TOMCAT/webapps directory. Note, in order for tomcat to be able to remember your configuration of deployed services etc. and reload it between reboots, you need to make the `axis/WEB-INF` directory writable. Settings will be maintained in a file called `server-config.xml`.
  1. Install database specific jar files i the $TOMCAT/common/lib directory:
    * mysql-connector-java-5.0.4.jar
    * naming-factory-dbcp.jar
  1. Restart tomcat. Note, while changes in the axis specific jar and configuration files will automatically be noticed by tomcat, always remember to restart tomcat after changing the `server.xml` file or adding common server jar files.
  1. Create the file `$TOMCAT/webapps/axis/META-INF/context.xml` and declare a resource for the database:
```
<Context reloadable="true" crossContext="true">

   <Resource name="jdbc/REMOTE"
             username="remote_admin"
             password="remote"
             url="jdbc:mysql://localhost/REMOTE?autoReconnect=true"
             maxActive="100" maxIdle="30" maxWait="10000"
             auth="Container"
             type="javax.sql.DataSource"
             driverClassName="com.mysql.jdbc.Driver"
             />

</Context>
```
  1. Check that the axis start page loads. If it does, also check that dependencies are satisfied by going to the "Validation" page.


---


# Installing the Testbed #

Before starting the installation, get the SourceCode. You should install the components in the following order: `remote-db`, `remote-ws`, `remote-mci`.

## Setting up the Database ##

First, create the database by running the command:
```
$ mysql < remote_core/scripts/create_remote_db.sql
```
as a user, who has write privileges, e.g. root. By default, the database will be named `REMOTE` with privileges granted to the user named `remote_admin` having the password `remote`. If you change the username and password (_which is highly recommended_), you should also change the `user_model/scripts/credentials.sh` script to mirror the new database credentials.

After creating the database, create the core testbed tables using the supplied script and specifying the username, password, and database on the command line:
```
$ mysql --user=remote_admin \
	--password=remote \
	REMOTE < remote_core/scripts/create_tables.sql
```

Finally, set up the site specific tables and attribute associations:
```
$ mysql --user=remote_admin \
	--password=remote \
	REMOTE < remote_core/scripts/insert_attribute_types.sql
```

Finally, create the tables for the user model:
```
$ mysql --user=remote_admin \
	--password=remote \
	REMOTE < user_model/scripts/create_tables.sql
```

## Building and Deploying the Web Services ##

First, build the server jar file by running `make` in the component's top-level directory.
```
$ make
```

Then copy the jar file to the appropriate `lib/` directory on your server:
```
$ scp lib/remote-ws-server.jar remote-server:$TOMCAT/webapps/axis/WEB-INF/lib
```
Verify in the log file that tomcat notices the new library and reloads the context. If you plan to do a little tweaking of the server-side code yourself, you can configure the `DEPLOYHOST` and `DEPLOYPATH` settings in the start of the `Makefile` to make the `deploy-jar` rule do this for you.

Request axis to deploy the the web services using the WSDD files in the `wsdd/` directory.
```
$ java org.apache.axis.client.AdminClient wsdd/*.wsdd
```
The above command assumes you have set the `CLASSPATH` environment to include the axis libraries. The `deploy-wsdd` rule in the `Makefile` might be of help, however, you need to change the default credentials in the top of the `Makefile`. This only needs to be done once.

Finally, verify that the 3 new web services are advertised by entering the URL of the axis start page in your browser and following the link to the list of deployed web services.

## Installing the Mote Control Host and Mote Control Server ##

If you are building from an SVN or git checkout, first run the bootstrap script to generate the build files.
```
$ ./bootstrap
```

Configure what you would like to build. Usually, it is enough to just run `configure`. Pass the `--help` option to list which options and environment variables can be used. The following example shows how to only build the mote control host program with the boost library installed in a custom location:
```
$ ./configure --prefix=/ --bindir=/sbin --disable-mcs CXXFLAGS=-I/usr/include/boost-1_33/
```
Verify that the configuration summary printed at the end of the script execution corresponds to your requirements.

Build all configured programs
```
$ make
```

To install on the local system to the location provided with the `--prefix` option, run:
```
$ make install
```
You should also install an start up script (e.g. into `/etc/init.d/`) so the mote control host and server is started when the system is booted. Examples for different systems are provided in the [contrib](http://remote-testbed.googlecode.com/svn/trunk/remote-mci/contrib/etc/init.d/) directory. Documentation on what command line options to use and how to configure the mote host and server is provided in the [remote-mch(1)](http://remote-testbed.googlecode.com/svn/documentation/remote-mci/remote-mch.1.html) and [remote-mcs(1)](http://remote-testbed.googlecode.com/svn/documentation/remote-mci/remote-mcs.1.html) man pages. The example configuration files for [remote-mch](http://remote-testbed.googlecode.com/svn/trunk/remote-mci/contrib/etc/remote-mch.cfg) and [remote-mcs](http://remote-testbed.googlecode.com/svn/trunk/remote-mci/contrib/etc/remote-mcs.cfg) in `contrib/` gives a good idea of the syntax.

On the system running the mote host, you will also need to configure the mote device discovery. The actions needed to be taken during the device discovery life-cycle are:

  * When a mote is added the mote host's device hierarchy should be be populated with symbolic links to the mote device's data and control TTY and files containing mote attributes.
  * When a mote is removed the mote attribute files should be removed.

Helper scripts exists for performing both of these tasks. See the [remote-device-add(7)](http://remote-testbed.googlecode.com/svn/documentation/remote-mci/remote-device-add.7.html) and [remote-device-remove(7)](http://remote-testbed.googlecode.com/svn/documentation/remote-mci/remote-device-remove.7.html) man pages for more information.

To perform these actions, you need to create UDEV rules for detecting when a mote device is added and when it is removed, as well as creating symbolic links and running external scripts. Since symbolic links should be created before the script to create the mote attribute files, the UDEV rule files should be numbered so that symlink rules have priority 65 and run rules have priorty 85. See the [README](http://remote-testbed.googlecode.com/svn/trunk/remote-mci/contrib/etc/udev/rules.d/README) and [example rules files](http://remote-testbed.googlecode.com/svn/trunk/remote-mci/contrib/etc/udev/rules.d/) for inspiration.


---


# Managing Users and Projects #

The **user\_model** module in the database component contains a series of scripts, which can be used for managing user and project information. Users and projects accounts are created separately and can afterward be associated by adding a user to a project allowing users to
be members of multiple different project.

Below is a small example where a user and a project is set up. It assumes that the `user_model/scripts/credentials.sh` file has been changed to automatically set database password. This can be done via something like the following:
```
export USER=remote_admin
export PASS=remote
export HOST=localhost
export MYSQL="mysql -u$USER -p$PASS -h$HOST"
export DBNAME=REMOTE
```

First, create a user:
```
$ ./user_model/scripts/create_user.sh
Enter login of new user:
fonseca
Enter password of new user:
secret
Enter email address of new user:
fonseca@diku.dk
Enter full name of new user:
Jonas Fonseca
insert into user(login,password,name,email) values ('fonseca',md5('secret'),'Jonas Fonseca','fonseca@diku.dk');
```

Next, create a project:
```
$ ./user_model/scripts/create_project.sh
Enter name of new project:
test
creating test;
insert into project(name) values ('test');
```

Finally, add the user to the project:
```
./user_model/scripts/add_user_project.sh
Enter login of user:
fonseca
Enter name of project:
test
insert into user_project(user_id,project_id) select (select id from user where login='fonseca'),(select id from project where name='test')
grant select,insert,delete,update on test.* to 'fonseca'@'%'
```

The remaining scripts can be used for changing a user password,
removing a user from a project, and deleting a user or project
account.