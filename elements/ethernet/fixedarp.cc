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

int FixedArp::configure(Vector<String> &conf, ErrorHandler *errh)
{
	ArgContext context(this);

	if (conf.size() != 2)
	{
		errh->error("expected SRC DST");
		return -1;
	}
	printf("conf len %d\n", conf.size());
	for (auto x: conf) {
		printf("conf %s\n", x.c_str());
	}
	if (!EtherAddressArg().parse(conf[0], src))
		return -1;

	if (!EtherAddressArg().parse(conf[1], dst))
		return -1;
	return 0;
}

Packet *FixedArp::simple_action(Packet *p)
{
	// make room for Ethernet header.
	WritablePacket *q = (WritablePacket *) p;
	q = (WritablePacket *) p->nonunique_push(sizeof(click_ether));
	q->set_mac_header(q->data());
	q->ether_header()->ether_type = htons(ETHERTYPE_IP);

	EtherAddress *dst_eth = reinterpret_cast<EtherAddress *>(q->ether_header()->ether_dhost);
	EtherAddress *src_eth = reinterpret_cast<EtherAddress *>(q->ether_header()->ether_shost);

	memcpy(&q->ether_header()->ether_dhost, dst.data(), 6);
	memcpy(&q->ether_header()->ether_shost, src.data(), 6);
	return q;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(FixedArp)
ELEMENT_MT_SAFE(FixedArp)
