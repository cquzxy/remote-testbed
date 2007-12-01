#ifndef _MOTE_H_
#define _MOTE_H_

#include "libgen.h"
#include "SerialControl.h"

namespace remote { namespace diku_mch {

class Mote : public SerialControl
{
	public:
		/** Create a new mote. */
		Mote(std::string& mac, std::string& directory);

		bool isValid();

		void invalidate();

		void validate();

		const std::string& getMac();
		const std::string& getDirectory();
		const std::string& getDevicePath();
		const std::string& getPlatform();
		std::string getImagePath();
		std::string getProgrammerPath();

	private:
		std::string readFile(std::string filename);
		std::string readLink(std::string linkname);
		std::string mac;
		std::string directory;
		std::string path;
		std::string platform;
		bool isvalid;
};

}}

#endif
