__________________________________________________________________________
__________________________________ HYDRA _________________________________
__________________________________________________________________________

hydra : Max/MSP / Pure data wrapper object for sixense SDK.
Compiled for and tested with Mac OSX 10.7
Copyright (C) 2012 Joseph Larralde
 
_________________________________ LICENSE ________________________________

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>. 

__________________________________ FLEXT _________________________________

This object needs flext framework built and installed in order to be built.
for more information see http://grrrr.org/research/software/flext

___________________________________ SDK __________________________________

To be built, it also needs an installation of the SDK from sixense which
is downloadable here : http://sixense.com/developers, and flext compiled
with links to it.

__________________________________________________________________________

To only use the binaries (.pd_darwin or .mxo), at least you need to
install the SDK.
Unzip the sixenseSDK_linux_OSX.zip file downloaded from the previously
mentioned address, open a terminal and cd to the newly created
sixenseSDK_linux_OSX folder, then to the install subfolder, then type :
python install.py
When asked, select "osx 32 bits" (choice #5). This script will install
sixense libraries in /usr/local/lib which must be there in order for the
hydra object to work.

__________________________________________________________________________
__________________________________________________________________________
__________________________________________________________________________