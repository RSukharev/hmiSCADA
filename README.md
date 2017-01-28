# hmiSCADA
Qt graph visualiser

hmiSCADA - is gui tool for visualising and managing applications with included program agents.
Visualisation of the input data requires a build of libraries: hmiSCADA/plugins/plugin_udp, hmiSCADA/plugins/plugin_syslog (plugin_syslog requires syslogparser.js in folder bin/plugins).
Another library hmiSCADA/plugins/testmodels/compmodel is used for demonstration of sending data to visualiser.

Libraries must be placed in bin/plugins folder. Also must be present bin/pixmaps with 3 pixmap files.

mini is chat program for testing of profiling and visualising fuctions and data flows.

Both applications can be compiled with Qt 4.8 and 5.6 on windows, macos and linux platforms.
