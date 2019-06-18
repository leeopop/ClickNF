#ifndef CLICK_FIXEDARP_HH
#define CLICK_FIXEDARP_HH
#include <click/element.hh>
#include <click/etheraddress.hh>
#include <click/ipaddress.hh>
CLICK_DECLS

class FixedArp : public Element { public:

    FixedArp() CLICK_COLD;
    ~FixedArp() CLICK_COLD;

    const char *class_name() const		{ return "FixedArp"; }
    const char *port_count() const		{ return "1/1"; }

    int configure(Vector<String> &, ErrorHandler *) CLICK_COLD;

    Packet *simple_action(Packet *p);

  private:
  EtherAddress src;
  EtherAddress dst;
};

CLICK_ENDDECLS
#endif
