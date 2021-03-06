#include "mcs/Host.h"

namespace remote { namespace mcs {

Host::Host( int fd, dbkey_t p_id, std::string ip, hostmapbykey_t& p_hostMap )
	: FileDescriptor(fd), id(p_id), message_in(), ipaddress(ip), hostMap(p_hostMap)
{
	hostMap[id] = this;
	protocols::setSendTimeout( fd,5,0); // 5 second host send timeout should be adequate
	// set keepalive options: 3 attempts after 120 seconds, 30 second interval
	setKeepAlive( fd, 3, 120, 30);
}

Host::~Host()
{
	Log::info("Closing host %s!", ipaddress.c_str());
	hostMap.erase(id);
}

void Host::destroy(bool silent)
{
	motemapbymac_t::iterator mI;

	Log::info("Deleting motes");
	for(mI = motesByMac.begin(); mI != motesByMac.end(); mI++)
		if (mI->second)
			mI->second->destroy(silent);
	delete this;
}

void Host::registerMoteByMac(std::string mac, Mote *mote)
{
	motemapbymac_t::iterator mI;

	mI = motesByMac.find(mac);

	if ( mI != motesByMac.end() )
	{
		mI->second->destroy();
	}

	motesByMac[mac] = mote;
}

void Host::deleteMoteByMac(std::string mac)
{
	motemapbymac_t::iterator mI;
	Mote* mote = NULL;
	mI = motesByMac.find(mac);

	if ( mI != motesByMac.end() )
	{
		mote = mI->second;
		motesByMac.erase(mI);
	}

	if (mote)
	{
		mote->destroy();
	}
}

void Host::findOrCreateMote(MsgMoteConnectionInfo& info)
{
	std::string path = info.getPath().getString();
	std::string mac = info.getMac();
	mysqlpp::Connection& sqlConn = dbConn.getConnection();
	dbkey_t site_id, mote_id;
	Mote* mote;
	mysqlpp::ResUse selectRes;
	mysqlpp::ResNSel execRes;
	mysqlpp::Row row;

	Log::info("Mote %s plugged at %s", mac.c_str(), path.c_str());

	mysqlpp::Query query = sqlConn.query();

	query << "select site_id from path"
		 " where host_id = " << id
	      << "   and path = " << mysqlpp::quote << path;

	// look for the connection path + host id to get the site_id
	selectRes = query.use();
	selectRes.disable_exceptions();
	row = selectRes.fetch_row();
	if (!row || row.empty()) {
		// if not found, create the path in the database with no site_id
		query.reset();
		query << "insert into path(host_id,path) "
	              << "values(" << id << "," << mysqlpp::quote << path << ")";

		query.execute(); // TODO: error checking

		// XXX: Set to the default site_id for new paths
		site_id = 1;

	} else {
		site_id = (dbkey_t) row["site_id"];
	}

	// look for the mac addresses in the database, get mote_id
	query.reset();
	query << "select mote_id from moteattr ma, mote_moteattr mma, moteattrtype mat"
		 " where ma.val=" << mysqlpp::quote << mac
	      << "   and mma.moteattr_id=ma.id"
		 "   and ma.moteattrtype_id=mat.id"
		 "   and mat.name='macaddress'";

	selectRes.purge();
	selectRes = query.use();
	selectRes.disable_exceptions();
	row = selectRes.fetch_row();

	MoteAddresses *newtarget = new MoteAddresses(mac);

	if ( !row || row.empty() )
	{
		std::ostringstream oss;

		selectRes.purge();
		// create the mote using site_id only - the mote class will create the
		// mote database record itself
		mote = new Mote(site_id, *this, *newtarget);
		// TODO: error checking
		// create the mac and net address database records using the mote id
		oss << (uint16_t) mote->mote_id;
		newtarget->netAddress = oss.str();

		Log::info("Attributes: macaddress=%s netaddress=%s platform=%s\n",
			  mac.c_str(), newtarget->netAddress.c_str(),
			  info.getPlatform().c_str());
		mote->setAttribute("macaddress", mac);
		mote->setAttribute("netaddress", newtarget->netAddress);
		mote->setAttribute("platform", info.getPlatform());
	}
	else
	{
		mote_id = (dbkey_t) row["mote_id"];   // save the mote_id
		selectRes.purge();
		// create the mote object using mote_id and site_id - the mote class will
		// update the mote database record to reflect the new site
		mote = new Mote(mote_id, site_id, *this, *newtarget);

		// get the net address mote attribute
		newtarget->netAddress = mote->getAttribute("netaddress");
		// TODO: error checking
	}

	if (mote)
	{
		// finally, register the new mote in the local map
		registerMoteByMac(mac, mote);
	}
}

void Host::handleMotesLostList(MsgMoteConnectionInfoList& infolist)
{
	MsgMoteConnectionInfo info;

	while (infolist.getNextMoteInfo(info))
		deleteMoteByMac(info.getMac());
}

void Host::handleMotesFoundList(MsgMoteConnectionInfoList& infolist)
{
	MsgMoteConnectionInfo info;

	while (infolist.getNextMoteInfo(info)) {
		findOrCreateMote(info);
	}
}

void Host::request(MoteAddresses& address, MsgPayload& request )
{
	MsgMoteAddresses addresses(address.mac, address.netAddress);
	MsgHostRequest msgHostRequest(addresses, request);
	HostMsg message(msgHostRequest);

	try {
		Message msg;
		msg.sendMsg(fd,message);
	}
	catch (remote::protocols::MMSException e)
	{
		Log::error("Exception: %s", e.what());
		this->destroy(true);
	}
}


void Host::handleEvent(short events)
{

	if ( (events & POLLIN) || (events & POLLPRI) )
	{
		if (!message_in.nonBlockingRecv(fd))
		{
			 return;
		}

		uint32_t msglen = message_in.getLength();
		uint8_t* buffer = message_in.getData();
		HostMsg message(buffer,msglen);

		switch (message.getType())
		{
			case HOSTMSGTYPE_HOSTCONFIRM:
				handleMsgIn(message.getHostConfirm());
				break;
			case HOSTMSGTYPE_PLUGEVENT:
				if ( message.getPlugEvent().getType() == PLUG_MOTES )
				{
					handleMotesFoundList(message.getPlugEvent().getInfoList());
				}
				else if ( message.getPlugEvent().getType() == UNPLUG_MOTES )
				{
					handleMotesLostList(message.getPlugEvent().getInfoList());
				}
				else
				{
					__THROW__ ("Invalid plugevent received from host!");
				}
				break;
			default:
				__THROW__ ("Invalid message type received from host!");
				break;
		}
	}
	else if ( (events & POLLERR) || (events & POLLHUP) || (events & POLLNVAL) )
	{
		__THROW__ ("Host connection error!\n");
	}
}

void Host::handleMsgIn(MsgHostConfirm& msgHostConfirm)
{
	motemapbymac_t::iterator mI;

	mI = motesByMac.find(msgHostConfirm.getMoteAddresses().getMac());
	if (mI != motesByMac.end()) {
		if (msgHostConfirm.getStatus() == MSGHOSTCONFIRM_UNKNOWN_MOTE) {
			deleteMoteByMac(msgHostConfirm.getMoteAddresses().getMac());
		} else {
			mI->second->confirm(msgHostConfirm.getMessage());
		}
	}
}

}}
