#include <click/config.h>
#include "fixedarp.hh"
#include <clicknet/ether.h>
#include <click/etheraddress.hh>
#include <click/ipaddress.hh>
#include <click/args.hh>
#include <click/bitvector.hh>
#include <click/straccum.hh>
#include <click/router.hh>
#include <click/error.hh>
#include <click/glue.hh>
#include <click/packet_anno.hh>
CLICK_DECLS

FixedArp::FixedArp()
{
}

FixedArp::~FixedArp()
{
}

int
FixedArp::configure(Vector<String> &conf, ErrorHandler *errh)
{
    ArgContext context(this);

	if (conf.size() != 2) {
		errh->error("expected SRC DST");
	    return -1;
	}
	if (EtherAddressArg().parse(conf[0], src, context))
			return -1;

	if (EtherAddressArg().parse(conf[1], dst, context))
			return -1;
	return 0;
}

Packet * FixedArp::simple_action(Packet *p)
{
   return p;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(FixedArp)
ELEMENT_MT_SAFE(FixedArp)
